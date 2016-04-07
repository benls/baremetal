.globl vector_table
.globl irq_handler
.globl vector_table_init

.bss
irq_lr_tmp: .word
irq_spsr_tmp: .word
irq_r0_tmp: .word

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
/* Save previous mode info */
ldr r13, =irq_lr_tmp
str lr, [r13,#0]
mrs lr, spsr
str lr, [r13,#4]
/* Switch to SVC mode with irq disabled*/
ldr lr, =0x193
msr cpsr, lr
/* push registers not saved by irq_handler*/
push {r0-r3,r12,lr}
/* push svc psr and svc lr */
ldr r2, =irq_lr_tmp
ldr r0, [r2]
ldr r1, [r2,#4]
push {r0,r1}
/* TODO: re-align stack on 64b boundary */
/* call handler */
blx irq_handler
/* disable interrupts */
ldr r0, =0x193
msr cpsr, r0
/* Save psr and lr */
ldr r2, =irq_lr_tmp
pop {r0,r1}
str r0, [r2]
str r1, [r2,#4]
/* save r0 */
ldr r0, [r13]
str r0, [r2,#8]
pop {r0-r3,r12,lr}
/* Switch back to irq mode */
ldr r0, =0x192
msr cpsr, r0
/* Restore previous mode info */
ldr r13, =irq_lr_tmp
ldr r0, [r13,#8]
ldr lr, [r13,#4]
msr spsr, lr
ldr lr, [r13]
/* Switch back to previous mode */
subs pc, lr, #4

vector_table_init:
ldr r1, =0x192
mrs r0, cpsr
msr cpsr, r1
msr cpsr, r0
bx lr

