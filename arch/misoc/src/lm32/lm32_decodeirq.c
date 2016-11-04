/********************************************************************************
 * arch/misoc/src/lm32/lm32_decodeirq.c
 *
 *   Copyright (C) 2016 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************/

/********************************************************************************
 * Included Files
 ********************************************************************************/

#include <nuttx/config.h>

#include "chip.h"
#include "lm32.h"

/********************************************************************************
 * Public Functions
 ********************************************************************************/

/****************************************************************************
 * Name: lm32_decodeirq
 *
 * Description:
 *   This function is called from the IRQ vector handler in lm32_vectors.S.
 *   At this point, the interrupt has been taken and the registers have
 *   been saved on the stack.  This function simply needs to determine the
 *   the irq number of the interrupt and then to call lm32_doirq to dispatch
 *   the interrupt.
 *
 *  Input parameters:
 *   regs - A pointer to the register save area on the stack.
 *
 ****************************************************************************/

uint32_t *lm32_decodeirq(uint32_t *regs)
{
  uint32_t intstat;
  int irq;

  /* Read the pending interrupts */
  /* REVISIT: How do I get the interupt status */
#warning Missing logic
  intstat = 0;

  /* REVIST: Do I need to mask the interrupt status with the IM? */

  irqinfo("intstat=%08lx\n", (unsigned long)intstat);

  /* Decode and dispatch interrupts */

  for (irq = 0; irq < MISOC_NINTERRUPTS & instat != 0; i++)
    {
      uint32_t bit = (1 << irq);

      /* Is this interrupt pending? */

      if ((instat & bit) != 0)
        {
          /* Yes.. Dispatch the interrupt */
          /* REVIST: Do I need to acknowledge the interrupt first? */

          irqinfo("irq=%d\n", irq);
          regs = lm32_doirq(irq, regs);

          /* Clear the bit in the interrupt status copy so that maybe we can
           * break out of the loop early.
           */

          instat &= ~bit;
        }
    }

  /* Return the final task register save area.  This will typically be the
   * same as the value of regs on input.  In the event of a context switch,
   * however, it will differ.  It will refere to the register save are in the
   * TCB of the new thread.
   */

  return regs;
}
