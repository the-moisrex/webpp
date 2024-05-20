import {Addenda, maskAddendum, positionAddendum, shiftAddendum} from "./modifiers.mjs";

const test = new Addenda("test", [positionAddendum, shiftAddendum, maskAddendum]);

console.log(test.render());

test.addenda.forEach(addendum => {
    if (!addendum.generable) {
        console.log(`------------ ${addendum.name}: Not generable ------------`);
        return;
    }
    console.log(`------------ ${addendum.name} ------------`);
    console.log(...addendum.generate());
});

console.log("----------------------");
