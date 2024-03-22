rm ./ircserv
cp ../bin/ircserv .

echo
echo "================ NOT ENOUGH/TOO MANY PARAMS =============="

echo
echo "./ircserv"
./ircserv

echo
echo "./ircserv ok ok ok"
./ircserv

echo
echo "================ INVALID PASSWORD =============="

echo
echo "./ircserv 6660 012345678901234567890123"
./ircserv 6660 012345678901234567890123

echo
echo '$> /ircserv 6661 "$(echo -e "hello\rworld")"'
./ircserv 6661 "$(echo -e "hello\rworld")"

echo
echo '$> /ircserv 6662 "$(echo -e "hello\nworld")"'
./ircserv 6662 "$(echo -e "hello\nworld")"

echo
echo '$> /ircserv 6663 "$(echo -e "hello\fworld")"'
./ircserv 6663 "$(echo -e "hello\fworld")"

echo
echo '$> /ircserv 6664 "$(echo -e "hello\tworld")"'
./ircserv 6664 "$(echo -e "hello\tworld")"

echo
echo '$> /ircserv 6666 "$(echo -e "hello\vworld")"'
./ircserv 6666 "$(echo -e "hello\vworld")"

echo
echo '$> /ircserv 6667 "hello world"'
./ircserv 6667 "hello world"

#  key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
#                   ; any 7-bit US_ASCII character,
#                   ; except NUL, CR, LF, FF, h/v TABs, and " "

# so try extended ascii (it(s until FF ))
echo
echo '$> /ircserv 6667 "çhello"'
./ircserv 6667 "çhello"

echo
echo '$> /ircserv 6667 ""'
./ircserv 6667 ""

echo
echo "================ INVALID TCP =============="

echo
echo "./ircserv zelfk pass"
./ircserv zelfk pass

echo
echo "./ircserv 6667zelfk pass"
./ircserv zelfk pass

echo
echo "./ircserv zelfk6667 pass"
./ircserv zelfk pass

echo
echo "./ircserv 0 pass"
./ircserv 0 pass

echo
echo "./ircserv 65536 pass"
./ircserv 65536 pass

echo
echo "./ircserv 1111111111111111111111111111111111 pass"
./ircserv 1111111111111111111111111111111111 pass
