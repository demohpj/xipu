[styling]
default=0x000000;0xffffff;false;false
comment=0x808080;0xffffff;false;false
number=0x007f00;0xffffff;false;false
string=0xff901e;0xffffff;false;false
operator=0x000000;0xffffff;false;false
identifier=0x880000;0xffffff;false;false
cpuinstruction=0x111199;0xffffff;true;false
mathinstruction=0x7f0000;0xffffff;true;false
register=0x000000;0xffffff;true;false
directive=0x3d670f;0xffffff;true;false
directiveoperand=0xff901e;0xffffff;false;false
commentblock=0x808080;0xffffff;false;false
character=0xff901e;0xffffff;false;false
stringeol=0x000000;0xe0c0e0;false;false
extinstruction=0x007f7f;0xffffff;false;false

[keywords]
instructions=nop str ldr ldf in out inc incc dec decc add addc sub subc not or and xor mov xchg loope loopz push pop swap shl shr clr call ret cmp jmp je jne jg jge jl jle halt
registers=
directives=external global var const include map

[settings]
extension=asm

comment_open=;
comment_close=
comment_use_indent=true
