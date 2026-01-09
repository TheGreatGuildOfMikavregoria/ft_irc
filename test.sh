#!/usr/bin/env bash
set -euo pipefail

HOST="127.0.0.1"
PORT="6667"
PASS="asd"

LOG_DIR="logs"
mkdir -p "$LOG_DIR"

# cleanup on exit
cleanup() {
  rm -f /tmp/c1.in /tmp/c2.in
  if [[ -n "${C1_PID:-}" ]]; then kill "$C1_PID" 2>/dev/null || true; fi
  if [[ -n "${C2_PID:-}" ]]; then kill "$C2_PID" 2>/dev/null || true; fi
}
trap cleanup EXIT

# named pipes for input
mkfifo /tmp/c1.in /tmp/c2.in
nc "$HOST" "$PORT" < /tmp/c1.in >> "$LOG_DIR/c1.log" &
C1_PID=$!
nc "$HOST" "$PORT" < /tmp/c2.in >> "$LOG_DIR/c2.log" &
C2_PID=$!

exec 3>/tmp/c1.in
exec 4>/tmp/c2.in

send_c1() { printf "%s\r\n" "$*" >&3; }
send_c2() { printf "%s\r\n" "$*" >&4; }


sleep 0.2

# register both
send_c1 "PASS $PASS"
send_c1 "NICK alice"
send_c1 "USER alice 0 * :Alice"

send_c2 "PASS $PASS"
send_c2 "NICK bob"
send_c2 "USER bob 0 * :Bob"

sleep 0.5

# ping/pong
send_c1 "PING 12345"
send_c2 "PING 67890"
sleep 0.5

# negative/edge cases (registration + basics)
send_c1 "NICK 123"                 # invalid (starts with digit)
send_c1 "NICK alice"               # duplicate nick (in use)
send_c1 "USER bad!name 0 * :Bad"   # invalid username (non-alnum)
send_c1 "PASS wrongpass"           # wrong PASS after registration
send_c1 "PING"                     # missing origin
sleep 0.5

# basic channel ops
send_c1 "JOIN #test"
send_c2 "JOIN #test"
sleep 0.5

# invalid channel names
send_c1 "JOIN test"                # missing '#'
send_c1 "JOIN #a"                  # too short? (allowed if length>=2)
send_c1 "JOIN #bad,name"           # invalid comma in name
sleep 0.5

# topic
send_c1 "TOPIC #test :Hello world"
sleep 0.5

# names / who
send_c1 "NAMES #test"
send_c1 "WHO #test"
send_c1 "WHO bob"
send_c1 "WHO nosuchnick"
sleep 0.5

# invite flow
send_c1 "JOIN #secret"
send_c1 "MODE #secret +i"
send_c1 "INVITE bob #secret"
sleep 0.5

send_c2 "JOIN #secret"
sleep 0.5

# channel modes
send_c1 "MODE #test +k keypass"
send_c1 "MODE #test +l 2"
send_c1 "MODE #test -k"
send_c1 "MODE #test -l"
send_c1 "MODE #test +z"            # unknown mode
sleep 0.5

# user mode +i and oper
send_c1 "MODE alice +i"
send_c1 "OPER ircAdmin admin@IRC42"
send_c1 "MODE bob +i"              # users-dont-match
sleep 0.5

# privmsg
send_c1 "PRIVMSG bob :hi bob"
send_c2 "PRIVMSG alice :yo"
send_c1 "PRIVMSG #test :hello channel"
send_c1 "PRIVMSG #secret :secret hello"
send_c1 "PRIVMSG"                  # no recipient
send_c1 "PRIVMSG bob"              # no text
send_c1 "PRIVMSG a,b :nope"         # too many targets
sleep 0.5

# kick / part / quit
send_c1 "MODE #test +o alice"
send_c1 "KICK #test bob :bye"
sleep 0.5

# invalid kick (not on channel or bad channel)
send_c1 "KICK #nosuch bob :x"
send_c2 "KICK #test alice :nope"
sleep 0.5

send_c2 "JOIN #test"
send_c2 "JOIN #secret"
sleep 0.5

# part all channels
send_c2 "JOIN 0"
sleep 0.5

send_c2 "PART #secret :later"
send_c1 "QUIT :bye"
send_c2 "QUIT :bye"

sleep 0.5
echo "Logs written to $LOG_DIR/c1.log and $LOG_DIR/c2.log"
