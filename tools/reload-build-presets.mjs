import fs from 'fs';

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
        .then(files => files.filter(file => file.endsWith("_test.cpp")).sort())
        .then(files => files.map(file => file.replace('_test.cpp', '')))
        .then(files => files.map(file => file.replaceAll('_', '-')));
}

async function getTestsNames() {
    return getTests()
        .then(names => names.map(name => `test-${name}`));
}

async function getExamples() {
    const examplesDir = `${projectRoot}examples`;
    const entries = await fs.promises.readdir(examplesDir, {
        withFileTypes: true
    });
    entries.sort((a, b) => (a.name < b.name ? -1 : a.name > b.name ? 1 : 0));
    const examples = [];
    for (const entry of entries) {
        if (!entry.isDirectory()) {
            continue;
        }
        const cmakeFile = `${examplesDir}/${entry.name}/CMakeLists.txt`;
        if (!fs.statSync(cmakeFile, {
                throwIfNoEntry: false
            })) {
            continue;
        }
        const content = await fs.promises.readFile(cmakeFile, 'utf8');
        const match = content.match(/^\s*set\(\s*EXEC_NAME\s+([A-Za-z0-9_-]+)/m);
        if (match) {
            examples.push(match[1]);
        }
    }
    return examples;
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
        configurePreset: "dev-default",
        hidden: true
    }];

    pr.testPresets = [{
        name: "test-default",
        configurePreset: "dev-default",
        hidden: true,
        output: {
            outputOnFailure: true
        },
        // The "tests" build preset only builds the per-test targets, so never run
        // the combined webpp-tests binary or the (clang-only) fuzz binaries here;
        // they are not built by that preset and would fail with "unable to find executable".
        filter: {
            exclude: {
                name: "^(webpp-tests|fuzz-.*)$"
            }
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

async function writeCMakePresets() {
    fs.writeFileSync(`${projectRoot}CMakePresets.json`, JSON.stringify(await reloadPresets(), null, 4));
}

await writeCMakePresets();
