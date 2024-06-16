() {
    profilestart();


    profilestop();

#ifdef _T9
    profilenamedstart("test profiler");


    profilenamedstop();
    
    profilenamedstart(#"test profiler 2");


    profilenamedstop();
#endif

}
