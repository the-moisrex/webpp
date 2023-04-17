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

    // remove example jobs (we remove them because one/some of them might be removed)
    actions.jobs = Object.fromEntries(Object.entries(actions.jobs)
            .filter(([name]) => !name.startsWith('example-')));


    actions.jobs = Object.fromEntries(Object.entries(actions.jobs)
            .filter(([name]) => !name.startsWith('build-')));

    actions.jobs = Object.fromEntries(Object.entries(actions.jobs)
            .filter(([name]) => !name.startsWith('run-')));

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
        }, {
            uses: 'awalsh128/cache-apt-pkgs-action@latest',
            with: {
                packages: 'libboost-all-dev zlib1g-dev googletest g++-12 ninja-build',
                version: 1.0
            }
        }
    ];


    actions.jobs['build-tests'] = {
        needs: 'install',
        'runs-on': 'ubuntu-latest',
        name: 'Build All Tests',
        steps: [
            ...jobDefaultSteps,
            {
                name: `Build All Tests`,
                run: `cmake --build --preset tests`
            }
        ]
    };

    actions.jobs['run-tests'] = {
        needs: 'build-tests',
        name: 'Run All Tests',
        'runs-on': 'ubuntu-latest',
        steps: [
            ...jobDefaultSteps,
            {
                name: `Run All Tests`,
                run: `ctest --preset tests`
            }
        ]
    };


    // Build tests
    for (const target of tests) {
        actions.jobs[`build-${target}`] = {
            // strategy: {
            //     'fail-fast': false,
            //     matrix: {
            //         target: [...tests]
            //     }
            // },
            name: `Build ${target}`,
            needs: 'install',
            'runs-on': 'ubuntu-latest',
            steps: [
                ...jobDefaultSteps,
                {
                    name: `Build ${target}`,
                    run: `cmake --build --preset ${target}`
                }
            ]
        };

        // Run tests
        actions.jobs[`run-${target}`] = {
            name: `Run ${target}`,
            needs: `build-${target}`,
            'runs-on': 'ubuntu-latest',
            steps: [
                ...jobDefaultSteps,
                {
                    name: `Run ${target}`,
                    run: `cmake --build --preset ${target}`
                },
                {
                    name: `Run ${target}`,
                    run: `./build/${target}`
                }
            ]
        };

    }

    // Run tests
    // actions.jobs['run-test-targets'] = {
    //     strategy: {
    //         'fail-fast': false,
    //         matrix: {
    //             target: [...tests]
    //         }
    //     },
    //     name: 'Run ${{ matrix.target }}',
    //     needs: 'build-test-target (${{ matrix.target }})',
    //     'runs-on': 'ubuntu-latest',
    //     steps: [
    //         ...jobDefaultSteps,
    //         {
    //             name: 'Build ${{ matrix.target }}',
    //             run: 'cmake --build --preset ${{ matrix.target }}'
    //         },
    //         {
    //             name: 'Run ${{ matrix.target }}',
    //             run: './build/${{ matrix.target }}'
    //         }
    //     ]
    // };



    actions.jobs['test-examples'] = {
        strategy: {
            'fail-fast': false,
            matrix: {
                target: [...examples]
            }
        },
        name: 'Build ${{ matrix.target }}',
        needs: 'install',
        'runs-on': 'ubuntu-latest',
        steps: [
            ...jobDefaultSteps,
            {
                name: 'Build Example ${{ matrix.target }}',
                run: 'cmake --build --preset ${{ matrix.target }}'
            }
            // ,{
            //     name: 'Run Example ${{ matrix.target }}',
            //     run: './build/${{ matrix.target }}'
            // }
        ]
    };


    actions.jobs.install = {
        'runs-on': 'ubuntu-latest',
        steps: [
            ...jobDefaultSteps,
            {
                name: `Configure CMake`,
                run: `cmake --preset=default`
            }
        ]
    };



    // actions.jobs['webpp-lib'] = {
    //     needs: 'install',
    //     'runs-on': 'ubuntu-latest',
    //     steps: [
    //         ...jobDefaultSteps,
    //         {
    //             name: `Build Web++ Library`,
    //             run: `cmake --build --preset webpp`
    //         }
    //     ]
    // };


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
