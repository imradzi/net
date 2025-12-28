file(GLOB net_SRC
    "*.cpp"
    "*.h"
)
add_library(net ${net_SRC})
SetIncludeDirectories(net)
