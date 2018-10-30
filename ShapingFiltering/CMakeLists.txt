project(CGraphShaping)
set(CMAKE_CXX_STANDARD 11)


add_executable(main main.cpp FlatSE.cpp cgraph.h
                    cgraphwatcher.h
                    FlatSE.h
                    Image.h
                    ragraph.h
                    ordering.h
                    shaping.h
                    tuple.h
                    Types.h
                    shaping.h
)