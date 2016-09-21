.globl vector_table
.globl irq_handler
.globl dabt_handler

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

