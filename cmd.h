//     name  num     arg	instrunctions_CPU 		insructions_disasm
DEF_CMD(HLT,  0,      0,
		{
			return;
		},
		{
			fprintf (stream, "HLT\n");
		}	
		)

DEF_CMD(PUSH, 1,      1,
		{
			GetArg (code, &ip, &arg);
			if (StackPush (stk, arg))
			{
				print_error("PUSH", ip);
				return;
			}
		},
		{
			fprintf (stream, "PUSH ");
			ConvertArg (code, &ip, stream);
		}
		)

DEF_CMD(POP,  2,      1,
		{
			GetPtrArg (code, &ip, &ptr_arg);
			if (StackPop (stk, ptr_arg))
			{
				print_error("POP", ip);
				return;
			}
		},
		{
			fprintf(stream, "POP ");
			ConvertArg (code, &ip, stream);
		}
		)

DEF_CMD(ADD,  3,      0,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs) || StackPush (stk, lhs + rhs))
			{
				print_error("ADD", ip);
				return;
			}
		},
		{
			fprintf(stream, "ADD\n");
		}
		)

DEF_CMD(SUB,  4,      0,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs) || StackPush (stk, lhs - rhs))
			{
				print_error("SUB", ip);
				return;
			}
		},
		{
			fprintf(stream, "SUB\n");
		}
		)

DEF_CMD(MULT, 5,      0,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs) || StackPush (stk, lhs * rhs))
			{
				print_error("MULT", ip);
				return;
			}
		},
		{
			fprintf(stream, "MULT\n");
		}
		)

DEF_CMD(DIV,  6,      0,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs) || StackPush (stk, lhs / rhs))
			{
				print_error("DIV", ip);
				return;
			}
		},
		{
			fprintf(stream, "DIV\n");
		}
		)

DEF_CMD(OUT,  7,      0,
		{
			if (StackPop (stk, &lhs))
			{
				print_error("OUT", ip);
				return;
			} else
			{
				printf (Elem_out, lhs);
				printf ("\n");
			}
		},
		{
			fprintf(stream, "OUT\n");
		}
		)

DEF_CMD(DUMP, 8,      0,
		{	
			char system_instruction[lenght_instruction] = "cat ";
			strcat(system_instruction, log_file_name);
			system (system_instruction);
		},
		{
			fprintf(stream, "DUMP\n");
		}
		)

DEF_CMD(IN,   9,      0,
		{
			while (!scanf (Elem_out, &lhs));
			if (StackPush (stk, lhs))
			{
				print_error("IN", ip);
				return;
			}
		},
		{
			fprintf(stream, "IN\n");
		}
		)

DEF_CMD(JMP,  16,     1,
		{
			ip = code[++ip] - 1;
		},
		{
			ip++;
			print_label(stream, "JMP", cnt_labels++);
		}
		)

DEF_CMD(JB,	  17,     1,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs))
			{
				print_error("JB", ip);
				return;
			} else
			if (lhs < rhs)
			{
				ip = code[++ip] - 1;		
			} else
				ip++;
		},
		{
			ip++;
			print_label(stream, "JB", cnt_labels++);
		}
		)

DEF_CMD(JBE,   18,     1,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs))
			{
				print_error("JBE", ip);
				return;
			} else
			if (lhs <= rhs)
			{
				ip = code[++ip] - 1;		
			} else
				ip++;
		},
		{
			ip++;
			print_label(stream, "JBE", cnt_labels++);
		}
		)

DEF_CMD(JA,   19,     1,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs))
			{
				print_error("JA", ip);
				return;
			} else
			if (lhs > rhs)
			{
				ip = code[++ip] - 1;		
			} else
				ip++;
		},
		{
			ip++;
			print_label(stream, "JA", cnt_labels++);
		}
		)

DEF_CMD(JAE,   20,     1,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs))
			{
				print_error("JAE", ip);
				return;
			} else
			if (lhs >= rhs)
			{
				ip = code[++ip] - 1;		
			} else
				ip++;
		},
		{
			ip++;
			print_label(stream, "JAE", cnt_labels++);
		}
		)

DEF_CMD(JE,   21,     1,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs))
			{
				print_error("JE", ip);
				return;
			} else
			if (lhs == rhs)
			{
				ip = code[++ip] - 1;		
			} else
				ip++;
			
		},
		{
			ip++;
			print_label(stream, "JE", cnt_labels++);
		}
		)

DEF_CMD(JNE,   22,     1,
		{
			if (StackPop (stk, &rhs) || StackPop (stk, &lhs))
			{
				print_error("JNE", ip);
				return;
			} else
			if (lhs != rhs)
			{
				ip = code[++ip] - 1;		
			} else
				ip++;
		},
		{
			ip++;
			print_label(stream, "JNE", cnt_labels++);
		}
		)

DEF_CMD(DUP,   10,     0,
		{
			if (StackPop(stk, &lhs) || StackPush(stk, lhs) || StackPush(stk, lhs))
			{
				print_error("DUP", ip);
				return;
			}
		},
		{
			fprintf(stream, "DUP\n");
		}
		)

DEF_CMD(CALL,  11,    1,
		{
			if (StackPush (stk_ret, ++ip))
			{
				print_error("CALL", ip);
				return;	
			}
			ip = code[ip] - 1;
		},	
		{
			ip++;
			fprintf(stream, "CALL f%d\n", data_of_funcs[cnt_funcs++].num);
		})

DEF_CMD(RET,   12,    0,
		{
			if (StackPop(stk_ret, &ip))
			{
				print_error("RET", ip);
				return;
			}
		},
		{
			fprintf(stream, "RET\n");	
		})