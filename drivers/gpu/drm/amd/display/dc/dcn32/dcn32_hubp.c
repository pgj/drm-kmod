/*
 * Copyright 2012-20 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: AMD
 *
 */

#include "dm_services.h"
#include "dce_calcs.h"
#include "reg_helper.h"
#include "basics/conversion.h"
#include "dcn32_hubp.h"

#define REG(reg)\
	hubp2->hubp_regs->reg

#define CTX \
	hubp2->base.ctx

#undef FN
#define FN(reg_name, field_name) \
	hubp2->hubp_shift->field_name, hubp2->hubp_mask->field_name

void hubp32_update_force_pstate_disallow(struct hubp *hubp, bool pstate_disallow)
{
	struct dcn20_hubp *hubp2 = TO_DCN20_HUBP(hubp);
	REG_UPDATE_2(UCLK_PSTATE_FORCE,
			DATA_UCLK_PSTATE_FORCE_EN, pstate_disallow,
			DATA_UCLK_PSTATE_FORCE_VALUE, 0);
}

void hubp32_update_mall_sel(struct hubp *hubp, uint32_t mall_sel)
{
	struct dcn20_hubp *hubp2 = TO_DCN20_HUBP(hubp);

	// Also cache cursor in MALL if using MALL for SS
	REG_UPDATE_2(DCHUBP_MALL_CONFIG, USE_MALL_SEL, mall_sel,
			USE_MALL_FOR_CURSOR, mall_sel == 2 ? 1 : 0);
}

void hubp32_prepare_subvp_buffering(struct hubp *hubp, bool enable)
{
	struct dcn20_hubp *hubp2 = TO_DCN20_HUBP(hubp);
	REG_UPDATE(DCHUBP_VMPG_CONFIG, FORCE_ONE_ROW_FOR_FRAME, enable);

	/* Programming guide suggests CURSOR_REQ_MODE = 1 for SubVP:
	 * For Pstate change using the MALL with sub-viewport buffering,
	 * the cursor does not use the MALL (USE_MALL_FOR_CURSOR is ignored)
	 * and sub-viewport positioning by Display FW has to avoid the cursor
	 * requests to DRAM (set CURSOR_REQ_MODE = 1 to minimize this exclusion).
	 *
	 * CURSOR_REQ_MODE = 1 begins fetching cursor data at the beginning of display prefetch.
	 * Setting this should allow the sub-viewport position to always avoid the cursor because
	 * we do not allow the sub-viewport region to overlap with display prefetch (i.e. during blank).
	 */
	REG_UPDATE(CURSOR_CONTROL, CURSOR_REQ_MODE, enable);
}

void hubp32_phantom_hubp_post_enable(struct hubp *hubp)
{
	uint32_t reg_val;
	struct dcn20_hubp *hubp2 = TO_DCN20_HUBP(hubp);

	REG_UPDATE(DCHUBP_CNTL, HUBP_BLANK_EN, 1);
	reg_val = REG_READ(DCHUBP_CNTL);
	if (reg_val) {
		/* init sequence workaround: in case HUBP is
		 * power gated, this wait would timeout.
		 *
		 * we just wrote reg_val to non-0, if it stay 0
		 * it means HUBP is gated
		 */
		REG_WAIT(DCHUBP_CNTL,
				HUBP_NO_OUTSTANDING_REQ, 1,
				1, 200);
	}
}

static struct hubp_funcs dcn32_hubp_funcs = {
	.hubp_enable_tripleBuffer = hubp2_enable_triplebuffer,
	.hubp_is_triplebuffer_enabled = hubp2_is_triplebuffer_enabled,
	.hubp_program_surface_flip_and_addr = hubp3_program_surface_flip_and_addr,
	.hubp_program_surface_config = hubp3_program_surface_config,
	.hubp_is_flip_pending = hubp2_is_flip_pending,
	.hubp_setup = hubp3_setup,
	.hubp_setup_interdependent = hubp2_setup_interdependent,
	.hubp_set_vm_system_aperture_settings = hubp3_set_vm_system_aperture_settings,
	.set_blank = hubp2_set_blank,
	.dcc_control = hubp3_dcc_control,
	.mem_program_viewport = min_set_viewport,
	.set_cursor_attributes	= hubp2_cursor_set_attributes,
	.set_cursor_position	= hubp2_cursor_set_position,
	.hubp_clk_cntl = hubp2_clk_cntl,
	.hubp_vtg_sel = hubp2_vtg_sel,
	.dmdata_set_attributes = hubp3_dmdata_set_attributes,
	.dmdata_load = hubp2_dmdata_load,
	.dmdata_status_done = hubp2_dmdata_status_done,
	.hubp_read_state = hubp3_read_state,
	.hubp_clear_underflow = hubp2_clear_underflow,
	.hubp_set_flip_control_surface_gsl = hubp2_set_flip_control_surface_gsl,
	.hubp_init = hubp3_init,
	.set_unbounded_requesting = hubp31_set_unbounded_requesting,
	.hubp_soft_reset = hubp31_soft_reset,
	.hubp_in_blank = hubp1_in_blank,
	.hubp_update_force_pstate_disallow = hubp32_update_force_pstate_disallow,
	.phantom_hubp_post_enable = hubp32_phantom_hubp_post_enable,
	.hubp_update_mall_sel = hubp32_update_mall_sel,
	.hubp_prepare_subvp_buffering = hubp32_prepare_subvp_buffering,
	.hubp_set_flip_int = hubp1_set_flip_int
};

bool hubp32_construct(
	struct dcn20_hubp *hubp2,
	struct dc_context *ctx,
	uint32_t inst,
	const struct dcn_hubp2_registers *hubp_regs,
	const struct dcn_hubp2_shift *hubp_shift,
	const struct dcn_hubp2_mask *hubp_mask)
{
	hubp2->base.funcs = &dcn32_hubp_funcs;
	hubp2->base.ctx = ctx;
	hubp2->hubp_regs = hubp_regs;
	hubp2->hubp_shift = hubp_shift;
	hubp2->hubp_mask = hubp_mask;
	hubp2->base.inst = inst;
	hubp2->base.opp_id = OPP_ID_INVALID;
	hubp2->base.mpcc_id = 0xf;

	return true;
}