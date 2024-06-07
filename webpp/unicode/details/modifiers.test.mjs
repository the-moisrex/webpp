import {genIndexAddenda, genMaskedIndexAddenda, ModifiedSpan, Modifier} from "./modifiers.mjs";
import {findSimilarRange, maxOf, overlapInserts} from "./utils.mjs";

const indexAddenda = genIndexAddenda();
const maskedIndexAddenda = genMaskedIndexAddenda();
indexAddenda.name = "test";
maskedIndexAddenda.name = "masked_test";

console.log(indexAddenda.render());
console.log(maskedIndexAddenda.render());

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
    pos: 0,
    shift: 0,
    mask: 255
});

console.log(nonChangingModifier);

const nonChangingSpan = new ModifiedSpan(table, nonChangingModifier);

console.log("length:", nonChangingSpan.length, table.length);
for (let index = 0; index !== table.length; index++) {
    const modified = nonChangingSpan.at(index);
    const orig = table.at(index);
    console.log(index, modified, orig, orig === modified);
}

console.log("--------------------------- Testing overlap ---------------------------");

const tab1 = [7, 8, 9, 10, 11, 12, 13, 14, 15, 16];
const tab2 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16];

const res = overlapInserts(tab1, tab2);
console.log('Found:', res, 'Real:', tab1.length);

console.log("--------------------------- Testing Similar Range Func ---------------------------");

const res2 = findSimilarRange(tab1, tab2);
console.log('Found:', res2, 'Real:', 6);

console.log("--------------------------- Testing maxof ---------------------------");
console.log(8, maxOf(8))
console.log(7, maxOf(7))
console.log(6, maxOf(6))
console.log(16, maxOf(16))
