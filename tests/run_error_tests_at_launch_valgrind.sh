rm valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv
cp ../bin/ircserv .

echo
echo "================ NOT ENOUGH/TOO MANY PARAMS =============="

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv ok ok ok"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv

echo
echo "================ INVALID PASSWORD =============="

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6660 012345678901234567890123"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6660 012345678901234567890123

echo
echo '$> /ircserv 6661 "$(echo -e "hello\rworld")"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6661 "$(echo -e "hello\rworld")"

echo
echo '$> /ircserv 6662 "$(echo -e "hello\nworld")"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6662 "$(echo -e "hello\nworld")"

echo
echo '$> /ircserv 6663 "$(echo -e "hello\fworld")"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6663 "$(echo -e "hello\fworld")"

echo
echo '$> /ircserv 6664 "$(echo -e "hello\tworld")"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6664 "$(echo -e "hello\tworld")"

echo
echo '$> /ircserv 6666 "$(echo -e "hello\vworld")"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6666 "$(echo -e "hello\vworld")"

echo
echo '$> /ircserv 6667 "hello world"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6667 "hello world"

#  key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
#                   ; any 7-bit US_ASCII character,
#                   ; except NUL, CR, LF, FF, h/v TABs, and " "

# so try extended ascii (it(s until FF ))
echo
echo '$> /ircserv 6667 "çhello"'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6667 "çhello"

echo
echo '$> /ircserv 6667 ""'
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6667 ""

echo
echo "================ INVALID TCP =============="

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv zelfk pass"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv zelfk pass

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6667zelfk pass"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv zelfk pass

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv zelfk6667 pass"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv zelfk pass

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 0 pass"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 0 pass

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 65536 pass"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 65536 pass

echo
echo "valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 1111111111111111111111111111111111 pass"
valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 1111111111111111111111111111111111 pass
