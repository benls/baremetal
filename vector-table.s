.globl vector_table
.globl irq_handler
.globl dabt_handler
.globl svc_table

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
b svc
/* prefetch abort */
prefetch:
b prefetch
/* data abort */
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
srsfd #0x13!
/* Switch to supervisor mode with irq disabled*/
cps #0x13
/* Push registers not saved by irq handler
 * Also save r4 to store the stack adjustment
*/
push {r0-r3,r4,r12,lr}
/* re-align stack on 64b boundary */
and r4, sp, #4
sub sp, sp, r4
/* call handler */
blx irq_handler
/* disable interrupts */
cpsid i
/* restore stack pointer */
add sp, sp, r4
/* return to previous mode */
pop {r0-r3,r4,r12,lr}
rfefd sp!

dabt:
/* Save previous mode info */
sub lr, lr, #4
srsfd #0x13!
/* Switch to supervisor mode with irq disabled*/
cps #0x13
/* push all other registers */
push {r0-r12}
/* save address of stored registers */
/* Stack now looks like:
 *    100
 *     FC SPSR
 *     F8 lr-dabt
 *     F4 r12
 *     ...
 * SP  C4 r0
*/
mov r0, sp
/* re-align stack on 64b boundary */
and r1, sp, #4
sub sp, sp, r1
push {r1, lr}
/* call handler */
blx dabt_handler
/* disable interrupts */
cpsid i
/* return to previous mode */
pop {r0, lr}
add sp, sp, r0
pop {r0-r12}
rfefd sp!

svc:
/* arguments: r0-r6 (r1-r6 saved)
 * syscall nr: r7 (saved)
 * return value: r0
*/
/* TODO: SYSCALL_LAST */
cmp r7, #0
bgt svc_bad_nr
/* Push registers not saved by swi handler
 * Also save r4 to store the stack adjustment
*/
push {r1-r3,r4,r12,lr}
/* get address of handler */
ldr r12, =svc_table
ldr r12, [r12, r7, LSL #2]
/* re-align stack on 64b boundary */
and r4, sp, #4
sub sp, sp, r4
/* call handler */
blx r12
/* restore stack pointer */
add sp, sp, r4
/* exit syscall */
ldmfd r13!, {r1-r3,r4,r12,pc}^
/* return -1 when the syscall nr is invalid */
svc_bad_nr:
ldr r0, =-1
/* exit syscall. TODO: push/pop */
push {lr}
ldmfd r13!, {pc}^

