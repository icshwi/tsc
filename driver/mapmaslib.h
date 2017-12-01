/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mapmaslib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : july 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    mapmaslib.c
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


#ifndef _H_MAPMASLIB
#define _H_MAPMASLIB

short tsc_map_mas_set_mode( struct tsc_device *ifc, struct tsc_ioctl_map_mode *m);
int tsc_map_mas_set_sg( struct tsc_device *ifc, struct map_ctl *map_ctl_p, int offset);
int tsc_map_mas_clear_sg( struct tsc_device *ifc, struct map_ctl *map_ctl_p, int offset, int npg);
int tsc_map_mas_alloc( struct tsc_device *ifc, struct tsc_ioctl_map_win *w);
int tsc_map_mas_modify( struct tsc_device *ifc, struct tsc_ioctl_map_win *w, ulong *rem_base);
int tsc_map_mas_free( struct tsc_device *ifc, int sg_id, uint offset);
int tsc_map_mas_get( struct tsc_device *ifc, struct tsc_ioctl_map_win *w);

#endif /*  _H_MAPMASLIB */

/*================================< end file >================================*/
