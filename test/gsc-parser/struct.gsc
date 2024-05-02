#namespace struct;

t() {
    return {
        #a: 42,
        #c: {
            #d: [],
            #e: [0,1,2,3,4]
        },
    };
}
t2() {
    return [
        12,
        {
            #a: {}
        },
        14,
        #"ate": 15,
        7: false
    ];
}