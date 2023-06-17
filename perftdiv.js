(async () => {
    const { spawnSync, spawn } = require('child_process');

    let depth = 5;
    let position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // // compile main.cpp
    // console.time("compile main.cpp");
    // let compile = spawnSync("g++ main.cpp -Wno-c99-designator -Wall -Wextra -pedantic -std=c++20 -O3 -Ofast -o main.exe",
    //     { shell: "powershell.exe", stdio: 'ignore' }
    // );
    // if (compile.status != 0) {
    //     console.error("compiler error");
    //     process.exit(1);
    // }
    // console.timeEnd("compile main.cpp");

    // run main.cpp perftdiv
    let a = spawnSync("main.exe", ["--perftdiv", depth, position]);
    perftC = Object.fromEntries(a.stdout.toString().split("\r\n").filter(e => e.includes(":")).map(e => [e.split(": ")[0], e.split(": ")[1]]));

    // run stockfish perftdiv
    let b = spawn("stockfish.exe");
    b.stdin.write(`position fen ${position}\ngo perft ${depth}\n`);
    b.stdin.end();
    let c = "";
    b.stdout.on("data", data => c += data);
    await new Promise(resolve => b.on("close", resolve));
    perftS = Object.fromEntries(c.split("\r\n").filter(e => !e.includes("Nodes")).filter(e => e.includes(":")).map(e => [e.split(": ")[0], e.split(": ")[1]]));

    console.log(c, a.stdout.toString());

    // check if all moves exist on both
    let d = Object.keys(perftC);
    let e = Object.keys(perftS);

    let f = d.filter(a => !e.includes(a));
    let g = e.filter(a => !d.includes(a));

    if (f.length > 0) {
        console.error(`illegal move: ${f}`);
    }
    if (g.length > 0) {
        console.error(`missing move: ${f}`);
    }


    // make sure all moves have the same amount of nodes on both
    let h = d.filter(a => perftC[a] != perftS[a]);
    console.log(h);
})();