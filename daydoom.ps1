param([Parameter(ValueFromRemainingArguments=$true)]$params)
& "$PSScriptRoot\build\Release\DayDoom.exe" @params
