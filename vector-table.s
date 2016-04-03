.globl vector_table
.globl irq_handler
.globl vector_table_init

.section ".bss"
.balign 64
irq_stack: .skip 512

.text
.balign 64
vector_table:
/* reset */
reset:
b reset
/* undefined inst */
undef:
b undef
/* SWI */
swi:
b swi
/* prefetch abort */
prefetch:
b prefetch
/* data abort */
dabt:
b dabt
/* reserved */
reserved:
b reserved
/* irq */
b irq
/* fiq */
fiq:
b fiq

irq:
push {r0-r3,r12,lr}
/* Switch to SVC mode */
//TODO: just us a constant
mrs r0, cpsr
bic r0, r0, #0x1f
orr r0, r0, #0x13
msr cpsr, r0
/* push registers not saved by irq_handler*/
push {lr}
/* TODO: re-align stack on 64b boundary */
blX irq_handler
pop {lr}
//TODO: just us a constant
mrs r0, cpsr
bic r0, r0, #0x1f
orr r0, r0, #0x12
msr cpsr, r0
pop {r0-r3,r12,lr}
subs pc, lr, #4

vector_table_init:
mrs r0, cpsr
mov r1, r0
//TODO: constant
bic r0, r0, #0x1f
orr r0, r0, #0x12
orr r0, r0, #0x1c0
msr cpsr, r0
ldr r13, =irq_stack
add r13, r13, #0x1fc
msr cpsr, r1
bx lr


