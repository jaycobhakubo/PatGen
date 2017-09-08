mkdir newpat
copy patlib.bin newpat /y
copy patlibh.bin newpat /y
copy games.dbf newpat /y
copy games.cdx newpat /y
copy c.out newpat /y
copy report.txt newpat /y
copy reporth.txt newpat /y
copy "Pattern_Compatibility_ Summary.htm" newpat /y
copy "NOTES.TXT" newpat /y


copy patlib.bin "\\atlas\engineering\Builds\Patterns\Latest" /y
copy patlibh.bin "\\atlas\engineering\Builds\Patterns\Latest" /y
copy games.dbf "\\atlas\engineering\Builds\Patterns\Latest" /y
copy games.cdx "\\atlas\engineering\Builds\Patterns\Latest" /y
copy c.out "\\atlas\engineering\Builds\Patterns\Latest" /y
copy report.txt "\\atlas\engineering\Builds\Patterns\Latest" /y
copy reporth.txt "\\atlas\engineering\Builds\Patterns\Latest" /y
copy "Pattern_Compatibility_ Summary.htm" "\\atlas\engineering\Builds\Patterns\Latest" /y
copy "NOTES.TXT" "\\atlas\engineering\Builds\Patterns\Latest" /y

\\atlas.dev.com\Engineering\Tools\Packager\1.0.4.0\packager.exe patlib.gtp -generate
copy "Patlib.gtipackage" "\\atlas\engineering\Builds\Patterns\Latest" /y
explorer.exe "\\atlas\engineering\Builds\Patterns\Latest"

rem copy patlib.bin "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy patlibh.bin "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy games.dbf "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy games.cdx "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy c.out "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy report.txt "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy reporth.txt "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy "Pattern_Compatibility_ Summary.htm" "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y
rem copy "NOTES.TXT" "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" /y

rem for kc local computer will delete if atlas is back
rem copy patlib.bin "D:\ToAndFrom\Patlib" /y
rem copy patlibh.bin "D:\ToAndFrom\Patlib" /y
rem copy games.dbf "D:\ToAndFrom\Patlib" /y
rem copy games.cdx "D:\ToAndFrom\Patlib" /y
rem copy c.out "D:\ToAndFrom\Patlib" /y
rem copy report.txt "D:\ToAndFrom\Patlib" /y
rem copy reporth.txt "D:\ToAndFrom\Patlib" /y
rem copy "Pattern_Compatibility_ Summary.htm" "D:\ToAndFrom\Patlib" /y
rem copy "NOTES.TXT" "D:\ToAndFrom\Patlib" /y

rem for jkn local computer will delete if atlas is back
rem copy patlib.bin "e:\atlas\patterns\latest" /y
rem copy patlibh.bin "e:\atlas\patterns\latest" /y
rem copy games.dbf "e:\atlas\patterns\latest" /y
rem copy games.cdx "e:\atlas\patterns\latest" /y
rem copy c.out "e:\atlas\patterns\latest" /y
rem copy report.txt "e:\atlas\patterns\latest" /y
rem copy reporth.txt "e:\atlas\patterns\latest" /y
rem copy "Pattern_Compatibility_ Summary.htm" "e:\atlas\patterns\latest" /y
rem copy "NOTES.TXT" "e:\atlas\patterns\latest" /y
rem e:\tools\packager\packager.exe patlib.gtp -generate
rem copy "Patlib.gtipackage" "e:\atlas\patterns\latest" /y
rem explorer.exe "e:\atlas\patterns\latest"

rem copy patlib.bin "R:\Releases\Patterns\latest" /y
rem copy patlibh.bin "R:\Releases\Patterns\latest" /y
rem copy games.dbf "R:\Releases\Patterns\latest" /y
rem copy games.cdx "R:\Releases\Patterns\latest" /y
rem copy c.out "R:\Releases\Patterns\latest" /y
rem copy report.txt "R:\Releases\Patterns\latest" /y
rem copy reporth.txt "R:\Releases\Patterns\latest" /y
rem copy "Pattern_Compatibility_ Summary.htm" "R:\Releases\Patterns\latest" /y
rem copy "NOTES.TXT" "R:\Releases\Patterns\latest" /y

rem explorer.exe "R:\Releases\Patterns"
rem \\Atlas\Data\Engineering\Packager\Packager.exe "\\atlas\Engineering\Builds\Elite\US\Patlib\Patlib.gtp" -generate 
rem explorer.exe "\\atlas\Engineering\Builds\Elite\US\Patlib\Latest" 
rem copy "\\atlas\Engineering\Builds\Elite\US\3.2\Development Builds\Tools\Patlib\Packages\Patlib.gtipackage" "R:\Releasesrem \Patterns\latest"
rem explorer.exe "R:\Releases\Patterns\latest"
