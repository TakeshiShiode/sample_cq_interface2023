function MatSetBLDCData(theta, vdc, Iu, Iw, omega)
    %theta_ptr = libpointer('doublePtr', theta);
    %vdc_ptr   = libpointer('doublePtr', vdc);
    %Iu_ptr    = libpointer('doublePtr', Iu);
    %Iw_ptr    = libpointer('doublePtr', Iw);
    %omega_ptr = libpointer('doublePtr', omega);
    %ret = 0;
    calllib('matlab_dll','MatlabSetBLDCData', theta, vdc, Iu, Iw, omega);
end