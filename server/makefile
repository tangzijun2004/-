.PHONY:cloud
cloud: cloud.cpp
	g++ -g $^ -o $@ -L./lib -lpthread -lstdc++fs -ljsoncpp -lbundle -Wno-register

.PHONY: clean
clean:
	rm -rf cloud