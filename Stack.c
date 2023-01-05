#include "Stack.h"

const char* Elem_out = "%d";
const Elem_t             POISON        = -1;
const int                MIN_SIZE_DATA = 4;
const char*              log_file_name = "log_file.txt";
FILE*                    log_stream    = nullptr;

#ifdef DUMPLING

const unsigned long long Canary        = 0xDEADF00D;

#endif

unsigned StackPush( stack* stk, Elem_t x)
{
	unsigned err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif
	
	if( err != STACK_OK)
		return err;

	if( err == STACK_OK)
	{
		if( stk->size < stk->capacity)
		{
			stk->data[stk->size++] = x;
		}
		else
		{
			err = StackResize( stk, STACK_UP);

			if( !err)
				stk->data[stk->size++] = x;

		}

		#ifdef DUMPLING

			stk->hash = Stack_hash_FAQ6(stk);

		#endif
	}

	if( err == STACK_OK)
		err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	return err;
}

unsigned StackPop( stack* stk, Elem_t* x)
{
	unsigned err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	if( err != STACK_OK)
		return err;
	
	if( err == STACK_OK)
	{
		if( stk->size == 0)
			err |= STACK_EMPTY;
		else
		{ 
			*x = stk->data[--stk->size];

			if( stk->size < stk->capacity / 4)
				StackResize( stk, STACK_DOWN);
		}

		#ifdef DUMPLING

			stk->hash = Stack_hash_FAQ6(stk);

		#endif
	}

	if( err == STACK_OK)
	{
		err = StackVerify( stk); 
	}

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	return err;
}

unsigned Stack_Ctor( stack** stk, size_t n, const char* name_stk, const char* name_of_file, const char* name_of_func, const int line)
{
	if( n < MIN_SIZE_DATA)
		n = MIN_SIZE_DATA;

	*stk = (stack*) calloc( 1, sizeof( **stk));

	unsigned err = STACK_OK;

	if( *stk != NULL)
	{
		(*stk)->data = (Elem_t*) calloc( n, sizeof( Elem_t));

		if( (*stk)->data != NULL)
		{
			for( size_t i = 0; i < n; i++)
				((*stk)->data)[i] = POISON;
			
			(*stk)->size = 0;
			(*stk)->capacity = n;

			#ifdef DUMPLING

				(*stk)->canary    = Canary;
				(*stk)->info.name = (*name_stk == '&') ? name_stk + 1 : name_stk;
				(*stk)->info.func = name_of_func;
				(*stk)->info.file = name_of_file;
				(*stk)->info.line = line;
				(*stk)->hash      = Stack_hash_FAQ6( *stk);

			#endif
		}
		
		err = StackVerify( *stk);
	}
	else
	{
		err |= HAS_NOT_MEMORY;
	} 

	return err;
}

unsigned StackDtor( stack* stk)
{
	unsigned err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	if( err == STACK_OK)
	{
		free( stk->data);
		stk->data     = (Elem_t*) POISON_DATA;
		stk->size     = POISON_SIZE;
		stk->capacity = POISON_CAPACITY;

		#ifdef DUMPLING

			stk->canary    = POISON_CANARY;
			stk->info.name = stk->info.func = stk->info.file = (const char*) POISON_NAME;
			stk->info.line = POISON_LINE;
			stk->hash      = POISON_HASH;

		#endif
	}

	return err;

}

unsigned StackVerify( stack* stk)
{
	unsigned err = STACK_OK;

	if( stk == NULL)
	{
		err |= STACK_NULLPTR;
		return err;
	} else
	{

		if( stk->data == NULL || stk->data == (Elem_t*) POISON_DATA)
			err |= STACK_NULL_DATA;
		else
		{
			if( stk->capacity < MIN_SIZE_DATA )
				err |= INVALID_CAPACITY;

			if( stk->size > stk->capacity && ((err & INVALID_CAPACITY) == 0))
				err |= INVALID_SIZE;

			if( (stk->hash != Stack_hash_FAQ6( stk)) || (stk->canary != Canary))
			{
				err |= STACK_IS_ATACKED;
			}

			if(err == STACK_OK)
			{
				for(size_t i = 0; i < stk->size; i++)
				{
					if(stk->data[i] == POISON || stk->data == NULL)
					{
						err |= INVALID_DATA;
						break;
					}
				}
			}
		}
	}

	return err;
}

unsigned StackResize( stack* stk, const int condition)
{
	unsigned err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	if( err != STACK_OK)
		return err;

	if( condition == STACK_UP)
	{
		if( stk->capacity < MIN_SIZE_DATA)
			stk->capacity = MIN_SIZE_DATA;
		else
			stk->capacity *= 2;

		stk->data = (Elem_t*) realloc( stk->data, stk->capacity * sizeof( Elem_t)); //ffff

		if( stk->data == NULL)
		{
			err |= HAS_NOT_MEMORY;
			return err;
		}

		#ifdef DUMPLING

			for( size_t i = stk->size; i < stk->capacity; i++)
				stk->data[i] = POISON;

		#endif
	}

	if( condition == STACK_DOWN)
	{
		if( stk->capacity >= 2*MIN_SIZE_DATA)
		{
			stk->capacity /= 2;

			stk->data = (Elem_t*) realloc( stk->data, stk->capacity * sizeof( Elem_t));
		}

		if( stk->data == NULL)
		{
			err |= HAS_NOT_MEMORY;
			return err;
		}

		#ifdef DUMPLING

			for( size_t i = stk->size; i < stk->capacity; i++)
				stk->data[i] = POISON;

		#endif
	}

	stk->hash = Stack_hash_FAQ6( stk);

	if( err == STACK_OK)
		err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	return err;
}

unsigned Printf_stk( stack* stk)
{
	unsigned err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	if( err != STACK_OK)
		return err;

	printf(	"Stack[%p](%s) \"%s\" at %s at %s(%d)\n"
			"{ \n"
			"    size =     %lu\n"
			"    capacity = %lu\n"
			"    stk->data[%p]\n"
			"    {\n",
			stk, (err == STACK_OK) ? "Ok" : "ERROR", stk->info.name, stk->info.func, stk->info.file, stk->info.line,
			stk->size, stk->capacity, stk->data);

	for( size_t i = 0; i < stk->capacity; i++)
	{
		if( i < stk->size)
			printf( "        *[%lu] = ", i);
		else
			printf( "         [%lu] = ", i);

		printf( Elem_out, stk->data[i]);
		printf( "\n");
	}

	printf( "    }\n"
			"}\n");

	err = StackVerify( stk);

	#ifdef DUMPLING

		StackDump(stk, err);

	#endif

	return err;	
}

#ifdef DUMPLING

void Stack_Dump( stack* stk, const unsigned err, const char* name_wrong_file, const char* name_wrong_func, int line)
{
	if( err != STACK_OK)
	{
		log_stream = fopen( log_file_name, "w");

		if( log_stream == NULL)
			return;

		if( err & HAS_NOT_MEMORY)
		{
			fprintf( log_stream, 
					"%s() at %s(%d):\n"
					"Not enough memory\n\n",
					name_wrong_func, name_wrong_file, line);

		} else
		{
			if( err & STACK_NULLPTR)
			{
				fprintf( log_stream,
					"%s() at %s(%d):\n"
					"Stack[%p]\n\n",
					name_wrong_func, name_wrong_file, line, stk);

			} else
			if( err & STACK_NULL_DATA)
			{
				fprintf( log_stream, 
					"%s() at %s(%d):\n"
					"Stack[%p](%s) \"%s\" at %s at %s(%d)\n"
					"{\n"
					"	stk->data[%p]\n"
					"	size =     %lu\n"
					"	capacity = %lu\n"
					"}\n\n",
					name_wrong_func, name_wrong_file, line,
					stk, (err == STACK_OK) ? "Ok" : "ERROR", stk->info.name, stk->info.func, stk->info.file, stk->info.line,
					stk->data, stk->size, stk->capacity);	
			} else
			if( (err & INVALID_CAPACITY) || (err & INVALID_SIZE))
			{
				fprintf( log_stream, 
					"%s() at %s(%d):\n"
					"Stack[%p](%s) \"%s\" at %s at %s(%d)\n"
					"{\n"
					"	stk->data[%p]\n"
					"	size =     %lu\n"
					"	capacity = %lu\n"
					"}\n\n",
					name_wrong_func, name_wrong_file, line,
					stk, (err == STACK_OK) ? "Ok" : "ERROR", stk->info.name, stk->info.func, stk->info.file, stk->info.line,
					stk->data, stk->size, stk->capacity);
			} else
			if( err & STACK_EMPTY)
			{
				fprintf( log_stream, 
					"%s at %s(%d):\n"
					"Stack[%p](%s) \"%s\" at %s at %s(%d)\n"
					"{"
					"	size =     %lu\n"
					"	capacity = %lu\n"
					"	stk->data[%p]\n"
					"	{\n",
					name_wrong_func, name_wrong_file, line,
					stk, (err == STACK_OK) ? "Ok" : "ERROR", stk->info.name, stk->info.func, stk->info.file, stk->info.line,
					stk->size, stk->capacity, stk->data);

				for( size_t i = 0; i < stk->capacity; i++)
				{
					if( i < stk->size)
						fprintf( log_stream, "		*[%lu] = ", i);
					else
						fprintf( log_stream, "		 [%lu] = ", i);

					fprintf( log_stream, Elem_out, stk->data[i]);
					fprintf( log_stream, "\n");
				}

				fprintf( log_stream, "	}\n"
									 "}\n");

			} else
			if( err & STACK_IS_ATACKED)
			{
				fprintf( log_stream, 
					"%s at %s(%d):\n"
					"Stack[%p](%s) \"%s\" at %s at %s(%d)\n"
					"{\n"
					"	size =     %lu\n"
					"	capacity = %lu\n"
					"	stk->data[%p]\n"
					"	{\n",
					name_wrong_func, name_wrong_file, line,
					stk, (err == STACK_OK) ? "Ok" : "ERROR", stk->info.name, stk->info.func, stk->info.file, stk->info.line,
					stk->size, stk->capacity, stk->data);

				for( size_t i = 0; i < stk->capacity; i++)
				{
					if( i < stk->size)
						fprintf( log_stream, "		*[%lu] = ", i);
					else
						fprintf( log_stream, "		 [%lu] = ", i);

					fprintf( log_stream, Elem_out, stk->data[i]);
					fprintf( log_stream, "\n");
				}

				fprintf( log_stream, "	}\n"
									 "}\n");
			} else
			{
				fprintf( log_stream, 
					"%s at %s(%d):\n"
					"Stack[%p](%s) \"%s\" at %s at %s(%d)\n"
					"{\n"
					"	size =     %lu\n"
					"	capacity = %lu\n"
					"	stk->data[%p]\n"
					"	{\n",
					name_wrong_func, name_wrong_file, line,
					stk, (err == STACK_OK) ? "Ok" : "ERROR", stk->info.name, stk->info.func, stk->info.file, stk->info.line,
					stk->size, stk->capacity, stk->data);

				for( size_t i = 0; i < stk->capacity; i++)
				{
					if( i < stk->size)
						fprintf( log_stream, "		*[%lu] = ", i);
					else
						fprintf( log_stream, "		 [%lu] = ", i);

					fprintf( log_stream, Elem_out, stk->data[i]);
					fprintf( log_stream, "\n");
				}

				fprintf( log_stream, "	}\n"
									 "}\n");
			}
		}

		fclose( log_stream);

	}
}

unsigned long long Stack_hash_FAQ6( const stack* stk)
{
	unsigned n = sizeof( stk) - sizeof(stk->hash);

	unsigned long long hash_data = 0;

	char* ptr = (char*) stk->data; 

	for (unsigned i = 0; i < stk->capacity * sizeof( Elem_t); i++)
    {
        hash_data += (unsigned char) (*(ptr+i));
        hash_data += (hash_data << 10);
        hash_data ^= (hash_data >> 6);
    }
    hash_data += (hash_data << 3);
    hash_data ^= (hash_data >> 11);
    hash_data += (hash_data << 15);

    unsigned long long hash_skt = 0;

    ptr = (char*) stk;

	for (unsigned i = 0; i < n; i++)
    {
        hash_skt += (unsigned char) (*(ptr+i));
        hash_skt += (hash_skt << 10);
        hash_skt ^= (hash_skt >> 6);
    }
    hash_skt += (hash_skt << 3);
    hash_skt ^= (hash_skt >> 11);
    hash_skt += (hash_skt << 15);     

    return hash_skt + hash_data;
}

#endif