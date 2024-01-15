--Normal pass, with bumpmapping
function normal		(shader, t_base, t_second, t_detail)
	local opt = shader:dx10Options()

	if not opt:texture_exist(t_base) then
		t_base = "ts_flora\\build_details"
	end

	local bump_texture = t_base.."_bump"
	if not opt:texture_exist(bump_texture) then
		bump_texture = "ts_flora\\build_details_bump"
	end

	if opt:texture_exist(bump_texture) then
		shader:begin	("deffer_grass","deffer_grass")	
		: fog (false)	
		shader:dx10stencil	(true, cmp_func.always, 255 , 127, stencil_op.keep, stencil_op.replace, stencil_op.keep)
		shader:dx10stencil_ref	(1)
		shader:dx10cullmode	(1)
		
		shader:dx10texture("s_base",	t_base)
		shader:dx10texture("s_bump",	bump_texture)
		shader:dx10texture("s_bumpX",	bump_texture.."#")
		
		shader:dx10sampler("smp_base")
	else
		shader:begin	("deffer_detail_w_flat","deffer_base_aref_flat")	
		: fog (false)	
		shader:dx10stencil	(true, cmp_func.always, 255 , 127, stencil_op.keep, stencil_op.replace, stencil_op.keep)
		shader:dx10stencil_ref	(1)
		shader:dx10cullmode	(1)
		
		shader:dx10texture("s_base",	t_base)
		
		shader:dx10sampler("smp_base")
	end
end