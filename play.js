// setTimeout(process.exit, 10 * 1000);

const spawn = require("child_process").spawn;
const net = require("net");
const fs = require("fs");

// get last edit from __filename
const lastEdit = fs.statSync(__filename).mtimeMs;

function play() {
    const isBlack = true;
    const isStupid = true;

    const sock = net.createConnection({
        host: "192.168.50.225",
        port: 1337
    });

    const engine = spawn("./main.exe", isBlack ? isStupid ? ["--black", "--ultrastupid"] : ["--black"] : isStupid ? ["--ultrastupid"] : [], {
        stdio: 'pipe',
    });

    let y = false;
    sock.on("data", (data) => {
        if (data.toString().includes("Result") || y) {
            // append to "results.txt"
            fs.appendFileSync("results.txt", data.toString());

            if (!y) {
                setTimeout(() => sock.end(), 50);
                engine.kill();
                setTimeout(() => fs.appendFileSync("results.txt", "\n"), 25);

                if (lastEdit == fs.statSync(__filename).mtimeMs) {
                    console.log("playing again!\n");
                    // if (data.toString().startsWith("Result")) {
                    setTimeout(play, 100);
                    // } else {
                    // console.log("bug found!");
                    // setInterval(() => { }, 1000);
                    // }
                } else {
                    console.log("restarting due to changes!\n");

                    setTimeout(() => process.exit(0), 1000);
                }
            }

            y = true;
        } else {
            process.stdout.write('s: ' + data.toString());
            engine.stdin.write(data);
            // sock.end();
        }
    });

    engine.stdout.on("data", (data) => {
        process.stdout.write('m: ' + data.toString());
        sock.write(data.toString().replace(/\r/g, ""));
        process.stdout.write(data);
    });
    engine.stderr.pipe(process.stderr);
}

// setTimeout(() => play, 1000 * 60 * 60 * 24 * 7);
play();