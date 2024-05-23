import {indexAddenda} from "./modifiers.mjs";

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
    1, 2, 3, 4, 5, 6, 7, 9, 10
];
for (const mod of indexAddenda.generate()) {
    console.log(mod, mod.apply(table, 0, 10));
}

