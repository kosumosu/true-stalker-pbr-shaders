function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("deffer_detail_w_flat","deffer_base_aref_flat")
			: fog		(false)

	local opt = shader:options()
	if not opt:texture_exist(t_base) then
		t_base = "ts_flora\\build_details"
	end

	shader:sampler	("s_base")      :texture	(t_base)
end
