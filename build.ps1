# g++ main.cpp -std=c++20 -o main
# echo "e2e4" | .\main %*

Function RunMyStuff {
    # this is the bit we want to happen when the file changes
    Clear-Host # remove previous console output
    g++ main.cpp -std=c++20 -o main && echo "e2e4" | .\main %*
}

Function Watch {    
    $global:FileChanged = $false # dirty... any better suggestions?
    $folder = "C:\Users\mieso\VSCode Projects\chessinator\"
    $filter = "main.cpp"
    $watcher = New-Object IO.FileSystemWatcher $folder, $filter -Property @{ 
        IncludeSubdirectories = $false 
        EnableRaisingEvents = $true
    }

    Register-ObjectEvent $Watcher "Changed" -Action {$global:FileChanged = $true} > $null

    while ($true){
        while ($global:FileChanged -eq $false){
            # We need this to block the IO thread until there is something to run 
            # so the script doesn't finish. If we call the action directly from 
            # the event it won't be able to write to the console
            Start-Sleep -Milliseconds 100
        }

        # a file has changed, run our stuff on the I/O thread so we can see the output
        RunMyStuff

        # reset and go again
        $global:FileChanged = $false
    }
}

RunMyStuff # run the action at the start so I can see the current output
Watch