gcc -o robin RoundRobinInterpreter.c Utils.c
gcc -o priority PriorityInterpreter.c Utils.c
gcc -o scheduler Scheduler.c Utils.c

cd bin

for file in *.c
do
	bin="${file%.*}"
	echo "Building $bin..."
	gcc -Wall -o $bin $file
done