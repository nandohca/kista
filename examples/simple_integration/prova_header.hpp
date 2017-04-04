



#define BEGIN_TASK_WITH_TOKEN_INOUT(task_name,BUFF_FROM,BUFF_TO,init_sec,...) 		\
    void task_name()					\
    {									\
	fifo_buffer<unsigned short> *in1;\
	fifo_buffer<unsigned short> *out1;\
	if(fifo_buffer_ushort.find("fifo"+std::to_string(BUFF_FROM))==fifo_buffer_ushort.end()) {\
		cout << task_name <<": Error retrieving fifo1 reference" << endl;\
		exit(-1);\
	}	\
	if(fifo_buffer_ushort.find("fifo"+std::to_string(BUFF_TO))==fifo_buffer_ushort.end()) {\
		cout << task_name <<": Error retrieving fifo1 reference" << endl;\
		exit(-1);\
	}\
	out1 = fifo_buffer_ushort["fifo"+std::to_string(BUFF_TO)];\
	in1 = fifo_buffer_ushort["fifo"+std::to_string(BUFF_FROM)];\
	\
	unsigned short in_var;\
	unsigned short out_var;										\
		init_sec;						\
		while(true) {                  \
		  in1->read(in_var);	\
	cout << task_name <<": recv: " << in_var << " at time " << sc_time_stamp() << endl;\
			__VA_ARGS__\
			
			
			
			
#define END_TASK_WITH_TOKEN_INOUT(task_name) \
	CONSUME_T;				\
	out_var=2;	\
	cout << task_name <<": send: " << out_var << " at time " << sc_time_stamp() << endl;\
	out1->write(out_var);\
 \
		  yield();						\
		}								\
     }

			
			
			
			
#define BEGIN_TASK_WITH_TOKEN_IN(task_name,BUFF_FROM,init_sec,...) 		\
    void task_name()					\
    {									\
		fifo_buffer<unsigned short> *in1;\
	if(fifo_buffer_ushort.find("fifo"+std::to_string(BUFF_FROM))==fifo_buffer_ushort.end()) {\
		cout << task_name <<": Error retrieving fifo1 reference" << endl;\
		exit(-1);\
	}	\
	in1 = fifo_buffer_ushort["fifo"+std::to_string(BUFF_FROM)];\
	\
	unsigned short in_var;\
		init_sec;						\
		while(true) {                  \
		  	in1->read(in_var);	\
			cout << task_name <<": recv: " << in_var << " at time " << sc_time_stamp() << endl;\
			__VA_ARGS__\
			
			
			
			
#define END_TASK_WITH_TOKEN_IN END_TASK_AFTER_WCET   //without the outgoing the normal consume_WCET and yield is enough
