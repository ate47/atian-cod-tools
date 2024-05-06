#using scripts\core_common\array_shared.gsc;
#using scripts\core_common\system_shared.gsc;
#using scripts\core_common\callbacks_shared.gsc;

#namespace acts;

function autoexec __init__system__() {
    level.acts = {
        #a: 42,
        #b: [1,2,3,4],
        #c: { #d: "bye", #e: "bye" }
    };
    test3 = "hello";
    test = 42;
    test2 = test;
}

function sum(array, count = array.size) {
    acc = 0;

    for (i = 0; i < count; i++) {
        acc += array[i];
    }
    
    return acc;
}