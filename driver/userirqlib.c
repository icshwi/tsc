/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : userirqlib.c
 *    author   : Oliver Talevski
 *    company  : ESS
 *    creation : Jan 17,2019
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the USER irq
 *    implemented on the TSC project.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *
 *    GPL license :
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *  
 *=============================< end file header >============================*/

#include "tscos.h"
#include "tscdrvr.h"

#define DBGno
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_user_irq
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure, source, argument
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : user interrupt handler
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void tsc_user_irq(struct  tsc_device *ifc, int src, void *arg)
{
	int ip, itc;
	struct user_irq_ctl *user_irq_ctl_p = (struct user_irq_ctl *)arg;

	ip = TSC_ALL_ITC_IACK_IP(src);
	itc = TSC_ALL_ITC_IACK_ITC(src);

	debugk(("Received interrupt %d from %d\n", ip, itc));
	up(&user_irq_ctl_p->user_irq_sem[ip]);

	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : user_irq_init
 * Prototype     : void
 * Parameters    : pointer to user irq control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : user interrupt init
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void user_irq_init(struct user_irq_ctl *user_irq_ctl_p)
{
	int irq = 0;

	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_0), TSC_ITC_SRC_USER1_0 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_1), TSC_ITC_SRC_USER1_1 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_2), TSC_ITC_SRC_USER1_2 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_3), TSC_ITC_SRC_USER1_3 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_4), TSC_ITC_SRC_USER1_4 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_5), TSC_ITC_SRC_USER1_5 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_6), TSC_ITC_SRC_USER1_6 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);
	tsc_irq_register(user_irq_ctl_p->ifc, ITC_CTL(TSC_ITC_SRC_USER1_7), TSC_ITC_SRC_USER1_7 & 0xf, tsc_user_irq, (void *)user_irq_ctl_p);

	for (irq = 0; irq < 8; irq++)
		sema_init(&user_irq_ctl_p->user_irq_sem[irq], 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_user_irq_wait
 * Prototype     : int
 * Parameters    : pointer to user irq control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : user interrupt wait
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_user_irq_wait(struct tsc_device *ifc, struct tsc_ioctl_user_irq *user_irq_p)
{
	int ret = 0, tmo, i, scale;
	int agent_sw_offset = 0;
	struct user_irq_ctl *user_irq_ctl_p;

	if (user_irq_p == NULL)
		return -EINVAL;

	if ((user_irq_p->irq < 0) || (user_irq_p->irq > 7))
		return -EINVAL;

	user_irq_ctl_p = ifc->user_irq_ctl;

	/* User irq are on agent sw 4, XUSER1 */
	agent_sw_offset = TSC_CSR_AGENT_SW_OFFSET * 4;

	tmo = (user_irq_p->wait_mode & 0xf0) >> 4;
	if (tmo)
	{
		int jiffies;

		i = (user_irq_p->wait_mode & 0x0e) >> 1;
		scale = 1;
		if (i)
		{
			i -= 1;
			while (i--)
			{
				scale = scale*10;
			}
		}
		jiffies = msecs_to_jiffies(tmo*scale) + 1;
		ret = down_timeout(&user_irq_ctl_p->user_irq_sem[user_irq_p->irq], jiffies);
	}
	else
	{
		ret = down_interruptible(&user_irq_ctl_p->user_irq_sem[user_irq_p->irq]);
	}

	if (ret)
	{
		printk("USER IRQ wait timeout\n");
		/* Mask interrupt */
		iowrite32((1 << user_irq_p->irq), ifc->csr_ptr + TSC_CSR_ITC_OFFSET + TSC_CSR_ITC_IMS_OFFSET + agent_sw_offset);
	}
	else
	{
		debugk(("USER IRQ received\n"));
	}

	return ret;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_user_irq_subscribe
 * Prototype     : int
 * Parameters    : pointer to user irq control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : user interrupt subscribe
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_user_irq_subscribe(struct tsc_device *ifc, struct tsc_ioctl_user_irq *user_irq_p)
{
	int agent_sw_offset = 0;
	struct user_irq_ctl *user_irq_ctl_p;

	if (user_irq_p == NULL)
		return -EINVAL;

	if ((user_irq_p->irq < 0) || (user_irq_p->irq > 7))
		return -EINVAL;

	user_irq_ctl_p = ifc->user_irq_ctl;
	sema_init(&user_irq_ctl_p->user_irq_sem[user_irq_p->irq], 0);

	/* User irq are on agent sw 4, XUSER1 */
	agent_sw_offset = TSC_CSR_AGENT_SW_OFFSET * 4;

	/* Unmask interrupt */
	iowrite32(user_irq_p->mask, ifc->csr_ptr + TSC_CSR_ITC_OFFSET + TSC_CSR_ITC_IMC_OFFSET + agent_sw_offset);

	return 0;
}

