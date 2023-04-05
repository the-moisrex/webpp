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


console.log(await reloadPresets())

fs.writeFileSync(`${projectRoot}CMakePresets.json`, JSON.stringify(await reloadPresets(), null, 4));
