ldr r1,=0x20200004
ldr r2,=0x2020001C
ldr r3,=0x20200028
mov r4,#1
lsl r4,#18
mov r5,#1
lsl r5,#16
loop:
str r4,[r1]
mov r6,#0x800000
str r5,[r3]
waitOff:
sub r6,r6,#1
cmp r6,#0
bne waitOff
str r5,[r2]
mov r6,#0x800000
waitOn:
sub r6,r6,#1
cmp r6,#0
bne waitOn
b loop
andeq r0,r0,r0
