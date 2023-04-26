import fs from 'fs';
import yaml from 'yaml';

const {
    projectRoot,
    presets
} = (() => {

    // find the json file
    let jsonFile = 'CMakePresets.json';
    let projectRoot = '';
    while (!fs.statSync(jsonFile, {
            throwIfNoEntry: false
        })) {
        jsonFile = '../' + jsonFile;
        projectRoot = '../' + projectRoot;
    }

    // read the file
    const presets = JSON.parse(fs.readFileSync(jsonFile))

    return {
        projectRoot,
        presets
    };
})();

async function getTests() {
    return fs.promises.readdir(`${projectRoot}tests`)
        .then(files => files.filter(file => file.endsWith("_test.cpp")))
        .then(files => files.map(file => file.replace('_test.cpp', '')))
        .then(files => files.map(file => file.replaceAll('_', '-')));
}

async function getTestsNames() {
    return getTests()
        .then(names => names.map(name => `test-${name}`));
}

async function getExamples() {
    return [
        'cgi-hello-world',
        'cgi-application',
        'json-app',
        'beast-json',
        'beast-view',
    ];
}

async function reloadPresets() {
    let pr = presets;
    const tests = await getTestsNames();
    const examples = await getExamples();
    const targets = [
        ...tests,
        ...examples,
        'webpp'
    ];
    pr.buildPresets = [{
        name: "default-build",
        configurePreset: "default",
        hidden: true
    }];

    pr.testPresets = [{
        name: "test-default",
        configurePreset: "default",
        hidden: true,
        output: {
            outputOnFailure: true
        },
        execution: {
            noTestsAction: "error",
            stopOnFailure: false
        }
    }];

    Array.prototype.push.apply(pr.testPresets, tests
        .map(target => ({
            name: target,
            inherits: "test-default"
        })));
    pr.testPresets.push({
        name: "tests",
        inherits: "test-default"
    });

    // All tests in one build
    pr.buildPresets.push({
        name: "tests",
        targets: tests,
        inherits: "default-build"
    });

    // Build Tests
    Array.prototype.push.apply(pr.buildPresets, tests
        .map(target => ({
            name: target,
            targets: target,
            inherits: "default-build"
        })));

    // benchmark
    pr.buildPresets.push({
        name: "benchmarks",
        targets: "webpp-benchmarks",
        inherits: "default-build"
    });

    // All Examples
    pr.buildPresets.push({
        name: "examples",
        targets: examples,
        inherits: "default-build"
    });

    // Examples
    Array.prototype.push.apply(pr.buildPresets, examples.map(target => ({
        name: target,
        targets: target,
        inherits: "default-build"
    })));

    return pr;
}

async function reloadGithubActions() {
    let actions = yaml.parse(fs.readFileSync(`${projectRoot}.github/workflows/build.yml`, 'utf8'));
    const tests = await getTestsNames();
    const examples = await getExamples();

    // remove test jobs (we remove them because one/some of them might be removed)
    actions.jobs = Object.fromEntries(Object.entries(actions.jobs)
            .filter(([name]) => !name.startsWith('test-')));

    const jobDefaultSteps = [
        {
            uses: "actions/checkout@v3"
        }, {
            uses: 'actions/cache@v3',
            with: {
                path: 'build',
                key: "${{ runner.os }}-${{ matrix.compiler }}-${{ env.BUILD_TYPE }}-${{ hashFiles('**/CMakeLists.txt') }}-${{ hashFiles('./CMakePresets.json')}}",
                'restore-keys': "${{ runner.os }}-${{ env.BUILD_TYPE }}-"
            }
        }
    ];

    // add tests jobs
    for (const target of tests) {
        actions.jobs[target] = {
            needs: 'install',
            'runs-on': 'ubuntu-latest',
            steps: [
                ...jobDefaultSteps,
                {
                    name: `Build ${target}`,
                    run: `cmake --build --preset ${target}`
                },
                {
                    name: `Run ${target}`,
                    run: `./build/${target}`
                }
            ]
        };
    }

    // remove example jobs (we remove them because one/some of them might be removed)
    actions.jobs = Object.fromEntries(Object.entries(actions.jobs)
            .filter(([name]) => !name.startsWith('example-')));

    // add examples
    for (const target of examples) {
        actions.jobs[`example-${target}`] = {
            needs: 'install',
            'runs-on': 'ubuntu-latest',
            steps: [
                ...jobDefaultSteps,
                {
                    name: `Build Example ${target}`,
                    run: `cmake --build --preset ${target}`
                },
                {
                    name: `Run Example ${target}`,
                    run: `./build/${target}`
                }
            ]
        };
    }

    actions.jobs.tests = {
        needs: 'install',
        'runs-on': 'ubuntu-latest',
        steps: [
            ...jobDefaultSteps,
            {
                name: `Build All Tests`,
                run: `cmake --build --preset tests`
            }, {
                name: `Run All Tests`,
                run: `ctest --preset tests`
            }
        ]
    };


    actions.jobs['webpp-lib'] = {
        needs: 'install',
        'runs-on': 'ubuntu-latest',
        steps: [
            ...jobDefaultSteps,
            {
                name: `Build Web++ Library`,
                run: `cmake --build --preset webpp`
            }
        ]
    };


    actions.jobs['benchmarks'] = {
        needs: 'install',
        'runs-on': 'ubuntu-latest',
        steps: [
            ...jobDefaultSteps,
            {
                name: `Build All Benchmarks`,
                run: `cmake --build --preset benchmarks`
            }, {
                name: "Run All Benchmarks",
                run: './build/webpp-benchmarks'
            }
        ]
    };

    return new yaml.Document(actions, {
        aliasDuplicateObjects: false // disabling anchors, Github Actions doesn't support it
    });
}

async function writeCMakePresets() {
    fs.writeFileSync(`${projectRoot}CMakePresets.json`, JSON.stringify(await reloadPresets(), null, 4));
}

async function writeGithubActions() {
    fs.writeFileSync(`${projectRoot}.github/workflows/build.yml`, yaml.stringify(await reloadGithubActions()));
}

// console.log(yaml.stringify(await reloadGithubActions()))

await writeCMakePresets();
await writeGithubActions();
