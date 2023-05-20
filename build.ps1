# g++ main.cpp -std=c++20 -o main
# echo "e2e4" | .\main %*

$file = (($args[0]) -like "*.cpp") ? $args[0] : "main.cpp"
$exe = ".\" + (($file -replace "\.\\", "") -Split "\.")[0] + ".exe"

# Clear-Host
while ($true) {
    # Clear-Host && clear && echo `r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n && Clear-Host && clear && g++ $file -Wno-c99-designator -Wall -Wextra -pedantic -std=c++20 -O3 -Ofast -o $exe -fsanitize=address -fsanitize=undefined -static-libsan -g && echo "b7b5" | &ptime $exe @((($args[0]) -like "*.cpp") ? ($args | Select-Object -Skip 1) : $args)
    # Clear-Host && clear && echo `r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n && Clear-Host && clear && g++ $file -Wno-c99-designator -Wall -Wextra -pedantic -std=c++20 -O3 -Ofast -o $exe && echo "b7b5" | &ptime $exe @((($args[0]) -like "*.cpp") ? ($args | Select-Object -Skip 1) : $args)
    # Clear-Host && clear && echo `r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n && Clear-Host && clear && g++ $file -Wno-c99-designator -Wall -Wextra -pedantic -std=c++20 -O3 -Ofast -o $exe -fsanitize=address -fsanitize=undefined -static-libsan -g && &ptime $exe @((($args[0]) -like "*.cpp") ? ($args | Select-Object -Skip 1) : $args)
    Clear-Host && clear && echo `r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n`r`n && Clear-Host && clear && g++ $file -Wno-c99-designator -Wall -Wextra -pedantic -std=c++20 -O3 -Ofast -o $exe && &ptime $exe @((($args[0]) -like "*.cpp") ? ($args | Select-Object -Skip 1) : $args)

    inotifywait.exe $file > $null 2> $null
    echo `r`nrecompiling...
}
