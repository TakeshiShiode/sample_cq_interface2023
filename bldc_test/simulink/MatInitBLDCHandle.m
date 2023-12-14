function  result = MatInitBLDCHandle()
    ret = 0;
    ret_ptr = libpointer('doublePtr', ret);
    loadlibrary('SCIBLDCINTERRUPT','sci_bldc_interrupt.h')
    calllib('SCIBLDCINTERRUPT','SciHandleInit',ret_ptr);
    result = ret_ptr.value;
end