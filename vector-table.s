.globl vector_table
.globl irq_handler

.data
irq_desired_pc:
.word 0
irq_desired_psr:
.word 0

.text
.balign 64
vector_table:
/* reset */
nop
/* undefined inst */
nop
/* SWI */
nop
/* prefetch abort */
nop
/* data abort */
nop
/* reserved */
nop
/* irq */
nop
/* fiq */
nop

irq:
/* debugging corrupts registers... */
ldr r13, =0x4804C194
ldr r12, =0x01e00000
str r12, [r13]
/* Save LR */
ldr r13, =irq_desired_pc
str lr, [r13]
/* Save SPSR */
mrs lr, spsr
ldr r13, =irq_desired_psr
str lr, [r13]
/* Switch to SVC mode */
mrs lr, cpsr
bic lr, lr, #0x1f
orr lr, lr, #0x13
mrs lr, cpsr
/* push registers not saved by irq_handler*/
push {r0-r3,r12,lr}
/* TODO: re-align stack on 64b boundary */
blx irq_handler
/* TODO... */

