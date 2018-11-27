#!/bin/sh

cp -v challenge.bin x.bin

function update_bytes()
{
    where=$1
    what=$2
    printf "$what" | dd bs=1 seek=$where of=x.bin conv=notrunc
}

update_bytes 10990 "\x07\x00"
update_bytes 10902 "\x07\x00"


