#!/bin/sh

read_cmus() {
    info=$(cmus-remote -Q)

    file=
    pos=

    while IFS= read -r line
    do
        case "$line" in
            file\ *)
                file=${line#file }
                ;;
            position\ *)
                pos=${line#position }
                ;;
        esac
    done <<EOF
$info
EOF
}

center() {
    text=$1

    cols=$(tput cols)

    len=${#text}
    pad=$(( (cols - len) / 2 ))

    [ "$pad" -lt 0 ] && pad=0

    printf "%*s%s\n" "$pad" "" "$text"
}

draw_lyrics() {
    lrc=${file%.*}.lrc

    [ -f "$lrc" ] || {
        center "$(cmus-remote -Q | sed -n 's/tag title //p')"
        return
    }

    gawk -v pos="$pos" '
    BEGIN {
        n = 0
    }

    function sec(s, a) {
        split(s, a, ":")
        return a[1] * 60 + a[2]
    }

    {
        if (!match($0, /^\[([0-9]+:[0-9.]+)\][[:space:]]*(.*)$/, m))
            next

        time[n]  = sec(m[1])
        lyric[n] = m[2]
        n++
    }

    END {
        current = -1

        for (i = 0; i < n; i++)
            if (time[i] <= pos)
                current = i

        if (current > 0)
            print " " lyric[current - 1]
        else
            print " "

        if (current >= 0)
            print "> " lyric[current]
        else
            print "> "

        if (current + 1 < n)
            print "  " lyric[current + 1]
        else
            print ""

        if (current + 2 < n)
            print " " lyric[current + 2]
        else
            print ""
    }
    ' "$lrc" |
    while IFS= read -r line
    do
        center "$line"
    done
}

clear_screen() {
    printf '\033[2J\033[H'
}

main() {
    while :
    do
        read_cmus
        clear_screen

        if [ -n "$file" ]; then
            draw_lyrics
        else
            center "CMUS is not running"
        fi

        sleep 0.25
    done
}

main
