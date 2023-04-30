# g++ main.cpp -std=c++20 -o main
# echo "e2e4" | .\main %*

$file = $args[0] -Contains ".cpp" ? $args[0] : "main.cpp"
$exe = ".\" + ($file -Split "\.")[0] + ".exe"

while ($true) {
    Clear-Host
    g++ $file -std=c++20 -o $exe && echo "e2e4" | &$exe @($args[0] -Contains ".cpp" ? ($args | Select-Object -Skip 1) : $args)
    .\inotifywait.exe $file 2> $null
}