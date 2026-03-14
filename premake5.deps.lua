group "Dependencies"
    for _, file in ipairs(os.matchfiles("premake/deps/*.lua")) do
        printf("Including dependency %s", file)
        include(file)
    end
    
    if _OPTIONS["gpl-build"] then
        for _, file in ipairs(os.matchfiles("premake/deps-gpl/*.lua")) do
            printf("Including GPL dependency %s", file)
            include(file)
        end
    end

