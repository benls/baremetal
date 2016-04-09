.globl vector_table
.globl irq_handler
.globl vector_table_init

.text
.balign 64
vector_table:
/* reset */
reset:
b reset
/* undefined inst */
undef:
b undef
/* SVC */
svc:
b svc
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
/* Save previous mode info */
sub lr, lr, #4
srsfd #0x1f!
/* Switch to system mode with irq disabled*/
cps #0x1f
/* push registers not saved by irq_handler*/
push {r0-r3,r12}
/* re-align stack on 64b boundary */
and r0, sp, #4
sub sp, sp, r0
push {r0, lr}
/* call handler */
blx irq_handler
/* disable interrupts */
cpsid i
/* return to previous mode */
pop {r0, lr}
add sp, sp, r0
pop {r0-r3,r12}
rfefd sp!

vector_table_init:
ldr r1, =0x192
mrs r0, cpsr
msr cpsr, r1
msr cpsr, r0
bx lr

