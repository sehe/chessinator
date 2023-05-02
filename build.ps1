# g++ main.cpp -std=c++20 -o main
# echo "e2e4" | .\main %*

$file = (($args[0]) -like "*.cpp") ? $args[0] : "main.cpp"
$exe = ".\" + (($file -replace "\.\\", "") -Split "\.")[0] + ".exe"

# Clear-Host
while ($true) {
    Clear-Host && g++ $file -Wall -Wextra -pedantic -std=c++20 -Ofast -o $exe -fsanitize=address -fsanitize=undefined -static-libsan -g && echo "e2e4" | &ptime $exe @((($args[0]) -like "*.cpp") ? ($args | Select-Object -Skip 1) : $args)
    inotifywait.exe $file > $null 2> $null
    echo `r`nrecompiling...
}