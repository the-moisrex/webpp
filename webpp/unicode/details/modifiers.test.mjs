import {genIndexAddenda, ModifiedSpan, Modifier} from "./modifiers.mjs";

const indexAddenda = genIndexAddenda();
indexAddenda.name = "test";

console.log(indexAddenda.render());

indexAddenda.addenda.forEach(addendum => {
    if (!addendum.generable) {
        console.log(`------------ ${addendum.name}: Not generable ------------`);
        return;
    }
    console.log(`------------ ${addendum.name} ------------`);
    console.log(...addendum.generate());
});

console.log("----------------------");

for (const value of indexAddenda.generate()) {
    console.log(value);
}

console.log("----------------------");

const table = [
    1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12
];
for (const mod of indexAddenda.generate()) {
    console.log(mod, mod.apply(table, 0, 10));
}

console.log("--------------------------- Testing Modified Span ---------------------------");

const nonChangingModifier = new Modifier(indexAddenda);
nonChangingModifier.set({
    shift: 0,
    mask: 255
});

console.log(nonChangingModifier);

const nonChangingSpan = new ModifiedSpan(table, 0, nonChangingModifier);

console.log("length:", nonChangingSpan.length, table.length);
for (let index = 0; index !== table.length; index++) {
    const modified = nonChangingSpan.at(index);
    const orig = table.at(index);
    console.log(index, modified, orig, orig === modified);
}
