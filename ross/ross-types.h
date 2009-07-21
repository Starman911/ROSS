#ifndef INC_ross_types_h
#define INC_ross_types_h

/*
 * ROSS: Rensselaer's Optimistic Simulation System.
 * Copyright (c) 1999-2003 Rensselaer Polytechnic Instutitute.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *
 *      This product includes software developed by David Bauer,
 *      Dr. Christopher D.  Carothers, and Shawn Pearce of the
 *      Department of Computer Science at Rensselaer Polytechnic
 *      Institute.
 *
 * 4. Neither the name of the University nor of the developers may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 5. The use or inclusion of this software or its documentation in
 *    any commercial product or distribution of this software to any
 *    other party without specific, written prior permission is
 *    prohibited.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

FWD(struct, tw_statistics);
FWD(struct, tw_pq);
FWD(struct, tw_lptype);
FWD(struct, tw_petype);
FWD(struct, tw_bf);
FWD(struct, tw_lp_state);
FWD(struct, tw_memoryq);
FWD(struct, tw_memory);
FWD(struct, tw_eventq);
FWD(struct, tw_event);
FWD(struct, tw_lp);
FWD(struct, tw_kp);
FWD(struct, tw_pe);
FWD(struct, tw_log);

/* 
 * tw_kpid -- Kernel Process (KP) id
 * tw_fd   -- used to distinguish between memory and event arrays
 */
typedef tw_peid tw_kpid;
typedef unsigned long tw_fd;
typedef long long tw_stat;

	/*
	 * User model implements virtual functions for per PE operations.  Currently,
	 * ROSS provides hooks for PE init, finalization and per GVT operations.
	 */
typedef void (*pe_init_f) (tw_pe * pe);
typedef void (*pe_gvt_f) (tw_pe * pe);
typedef void (*pe_final_f) (tw_pe * pe);

	/* pre_lp_init -- PE initialization routine, before LP init.
	 * post_lp_init --  PE initialization routine, after LP init.
	 * gvt		-- PE per GVT routine.
	 * final	-- PE finilization routine.
	 */
DEF(struct, tw_petype)
{
	pe_init_f pre_lp_init;
	pe_init_f post_lp_init;
	pe_gvt_f gvt;
	pe_final_f final;
};

	/*
	 * User implements virtual functions by giving us function pointers
	 * for setting up an LP, handling an event on that LP, reversing the
	 * event on the LP and cleaning up the LP for stats computation/collecting
	 * results.
	 */
typedef void (*init_f) (void *sv, tw_lp * me);
typedef tw_peid (*map_f) (tw_lpid);
typedef void (*event_f) (void *sv, tw_bf * cv, void *msg, tw_lp * me);
typedef void (*revent_f) (void *sv, tw_bf * cv, void *msg, tw_lp * me);
typedef void (*final_f) (void *sv, tw_lp * me);
typedef void (*statecp_f) (void *sv_dest, void *sv_src);

	/*
	 *  init        -- LP setup routine.
	 *  map		-- LP mapping of LP gid -> remote PE routine.
	 *  event       -- LP event handler routine.
	 *  revent      -- LP RC event handler routine.
	 *  final       -- LP final handler routine.
	 *  statecp     -- LP SV copy routine.
	 *  state_sz    -- Number of bytes that SV is for the LP.
	 */
DEF(struct, tw_lptype)
{
	init_f init;
	event_f event;
	revent_f revent;
	final_f final;
	map_f map;
	size_t state_sz;
};

DEF(struct, tw_statistics)
{
	double max_run_time;

	tw_stat tw_s_net_events;
	tw_stat tw_s_nevent_processed;
	tw_stat tw_s_nevent_abort;
	tw_stat tw_s_e_rbs;
	tw_stat tw_s_rb_total;
	tw_stat tw_s_rb_primary;
	tw_stat tw_s_rb_secondary;
	tw_stat tw_s_fc_attempts;
	tw_stat tw_s_pq_qsize;

	tw_stat tw_s_nsend_network;
	tw_stat tw_s_nrecv_network;

	tw_stat tw_s_nsend_remote_rb;
	tw_stat tw_s_nsend_loc_remote;
	tw_stat tw_s_nsend_net_remote;
};

DEF(struct, tw_memoryq)
{
	int		 size;
	unsigned int	 start_size;
	unsigned int	 d_size;
	unsigned int	 grow;

	tw_memory	*head;
	tw_memory	*tail;
};

	/*
	 * tw_memory
	 * 
	 * This is a memory buffer which applications can use in any way they
	 * see fit.  ROSS provides API methods for handling memory buffers in the event of
	 * a rollback and manages the memory in an efficient way, ie, like events.
	 */
DEF(struct, tw_memory)
{
	/*
	 * next		-- Next pointer for all queues except the LP RC queue
	 * prev		-- Prev pointer for all queues except the LP RC queue
	 * lp_next	-- Next pointer for the LP processed memory queue
	 * pe		-- PE which originally alloc'ed this buffer: needed in order
	 *		   to return buffer to the PE that created it
	 * ts		-- time at which this event can be collected
	 * data		-- data segment of the memory buffer: application defined
	 */
	tw_memory	*volatile next;
	tw_memory	*volatile prev;
	//tw_memory	*volatile up;
	tw_memory	*volatile lp_next;

	//int		 heap_index;
	tw_stime         ts;

	int		 bit;
	void		*data;
};

DEF(struct, tw_eventq)
{
	size_t size;
	tw_event *volatile head;
	tw_event *volatile tail;
};

	/*
	 * Some applications find it handy to have this bitfield when doing
	 * reverse computation.  So we follow GTW tradition and provide it.
	 */
DEF(struct, tw_bf)
{
	unsigned int    c0:1;
	unsigned int    c1:1;
	unsigned int    c2:1;
	unsigned int    c3:1;
	unsigned int    c4:1;
	unsigned int    c5:1;
	unsigned int    c6:1;
	unsigned int    c7:1;
	unsigned int    c8:1;
	unsigned int    c9:1;
	unsigned int    c10:1;
	unsigned int    c11:1;
	unsigned int    c12:1;
	unsigned int    c13:1;
	unsigned int    c14:1;
	unsigned int    c15:1;
	unsigned int    c16:1;
	unsigned int    c17:1;
	unsigned int    c18:1;
	unsigned int    c19:1;
	unsigned int    c20:1;
	unsigned int    c21:1;
	unsigned int    c22:1;
	unsigned int    c23:1;
	unsigned int    c24:1;
	unsigned int    c25:1;
	unsigned int    c26:1;
	unsigned int    c27:1;
	unsigned int    c28:1;
	unsigned int    c29:1;
	unsigned int    c30:1;
	unsigned int    c31:1;
};

	/*
	 * tw_lp_state:
	 *
	 * Used to make a list of LP state vectors.  The entire state
	 * is usually going to always be larger than this object, but
	 * the minimum size is this object.  When the state is a
	 * tw_lp_state it does NOT hold any application data, so we
	 * can overwrite it with our own safely.
	 */
DEF(struct, tw_lp_state)
{
	tw_lp_state    *next;
};

FWD(enum, tw_event_owner);
DEF(enum, tw_event_owner)
{
	TW_pe_event_q = 1,	/* In a tw_pe.event_q list */
	TW_pe_pq = 2,		/* In a tw_pe.pq */
	TW_kp_pevent_q = 3,     /* In a tw_kp.pevent_q */
	TW_pe_anti_msg = 4,     /* Anti-message */
	TW_net_outq = 5,        /* Pending network transmission */
	TW_net_asend = 6,       /* Network transmission in progress */
	TW_net_acancel = 7,     /* Network transmission in progress */
	TW_pe_sevent_q = 8,     /* In tw_pe.sevent_q */
	TW_pe_free_q = 9        /* In tw_pe.free_q */
};

	/*
	 * tw_event:
	 *
	 * Holds entire event structure, one is created for each and every
	 * event in use.
	 */
DEF(struct, tw_event)
{
	tw_event *tw_volatile next;
	tw_event *tw_volatile prev;
#ifdef ROSS_QUEUE_splay
	tw_event *tw_volatile up;
#endif
#ifdef ROSS_QUEUE_heap
	unsigned long heap_index;
#endif

	/* cancel_next  -- Next event in the cancel queue for the dest_pe.
	 * caused_by_me -- Start of event list caused by this event.
	 * cause_next   -- Next in parent's caused_by_me chain.
	 */
	tw_event *tw_volatile cancel_next;
	tw_event *tw_volatile caused_by_me;
	tw_event *tw_volatile cause_next;

	int		 color;
	tw_eventid	 event_id;

	/* Status of the event's queue location(s). */
	struct
	{
		BIT_GROUP(

		/* Owner of the next/prev pointers; see tw_event_owner */
		BIT_GROUP_ITEM(owner, 4)

		/* Actively on a dest_lp->pe's cancel_q */
		BIT_GROUP_ITEM(cancel_q, 1)
		BIT_GROUP_ITEM(cancel_asend, 1)

		/* Indicates union addr is in 'remote' storage */
		BIT_GROUP_ITEM(remote_fmt, 1)
		)
	} state;

	/* cv -- Used by app during reverse computation.
	 * lp_state -- dest_lp->state BEFORE this event.
	 */
	tw_bf		 cv;
	//void		*lp_state;

	/* dest_lp -- Destination LP object.
	 * src_lp -- Sending LP.
	 * recv_ts -- Actual time to be received.
	 * event_id -- Unique id assigned by src_lp->pe if remote.
	 */
	tw_lp		*dest_lp;
	tw_lp		*src_lp;
	tw_stime	 recv_ts;

	tw_peid		 send_pe;

	tw_memory	*memory;
};

	/*
	 * tw_lp:
	 *
	 * Holds our state for the LP, including the lptype and a pointer
	 * to the user's current state.  The lptype is copied into the tw_lp
	 * in order to save the extra memory load that would otherwise be
	 * required (if we stored a pointer).
	 *
	 * Specific PE's service specific LPs, each PE has a linked list of
	 * the LPs it services, this list is made through the pe_next field
	 * of the tw_lp structure.
	 */
DEF(struct, tw_lp)
{
	// local LP id
	tw_lpid id;

	// global LP id
	tw_lpid gid;

	tw_pe *pe;

	/* kp -- Kernel process that we belong to (must match pe).
	 * pe_next  -- Next LP in the PE's service list.
	 */
	tw_kp *kp;
	//tw_lp *pe_next;

	/* cur_state -- Current application LP data.
	 * state_qh -- Head of [free] state queue (for state saving).
	 * type -- Type of this LP, including service callbacks.
	 */
	void		*cur_state;
	tw_lp_state	*state_qh;
	tw_lptype	 type;
	tw_rng_stream	*rng;
};

	/*
	 * tw_kp:
	 *
	 * Holds our state for the Kernel Process (KP), which consists only of
	 * processed event list for a collection of LPs.  
	 */
DEF(struct, tw_kp)
{
	/* id -- ID number, otherwise its not available to the app.
	 * pe -- PE that services this KP.
	 * next -- Next KP in the PE's service list.
	 */
	tw_kpid id;
	tw_pe *pe;
	tw_kp *next;

	/* last_time -- Time of the current event being processed.
	 * pevent_q -- Events processed by LPs bound to this KP
	 */
	tw_stime last_time;
	tw_eventq pevent_q;

	/*
	 * queues -- TW memory buffer queues
	 */
	tw_memoryq	*queues;

	/* s_nevent_processed -- Number of events processed.
	 * s_e_rbs -- Number of events rolled back by this LP.
	 * s_rb_total -- Number of total rollbacks by this LP.
	 * s_rb_secondary -- Number of secondary rollbacks by this LP.
	 */
	tw_stat s_nevent_processed;
	tw_stat s_mem_buffers_used;
	long s_e_rbs;
	long s_rb_total;
	long s_rb_secondary;
};

	/*
	 * tw_pe
	 *
	 * Holds the entire PE state.  We slug a pointer to this around quite
	 * often, as it gives us the thread's local data (essentially).
	 */
DEF(struct, tw_pe)
{
	tw_peid id;
	tw_node	node;

	/* type -- Model defined PE type routines.
	 */
	tw_petype type;

	/* event_q -- Linked list of events sent to this PE.
	 * cancel_q -- List of canceled events.
	 * event_q_lck -- processor specific lock for this PE's event_q.
	 * cancel_q_lck -- processor specific lock for this PE's cancel_q.
	 * pq -- Priority queue used to sort events.
	 * lp_list -- List of LPs this PE services.
	 * kp_list -- List of KPs this PE services.
	 * pe_next -- Single linked list of PE structs.
	 * rollback_q -- List of KPs actively rolling back.
	 */
	tw_eventq event_q;
	tw_event *tw_volatile cancel_q;
	tw_mutex event_q_lck;
	tw_mutex cancel_q_lck;
	tw_pq *pq;
	//tw_lp *lp_list;
	//tw_kp *kp_list;
	tw_pe **pe_next;

	/* free_q -- Linked list of free tw_events.
	 * abort_event -- Placeholder event for when free_q is empty.
	 * cur_event -- Current event being processed.
	 * sevent_q -- events already sent over the network.
	 */
	tw_eventq free_q;
	tw_event *abort_event;
	tw_event *cur_event;
	tw_eventq sevent_q;

	/* clock_offset -- Initial clock value for this PE.
	 * clock_time -- Most recent clock value for this PE.
	 */
	tw_clock clock_offset;
	tw_clock clock_time;

	/* cev_abort	-- Current event being processed must be aborted.
	 * master	-- Master across all compute nodes.
	 * local_master -- Master for this node.
	 * gvt_status	-- bits available for gvt computation.
	 */
	BIT_GROUP(
	BIT_GROUP_ITEM(cev_abort, 1)
	BIT_GROUP_ITEM(master, 1)
	BIT_GROUP_ITEM(local_master, 1)
	BIT_GROUP_ITEM(gvt_status, 4)
	)

	/* trans_msg_ts -- Last transient messages' time stamp.
	 * GVT -- global virtual time
	 * LVT -- local (to PE) virtual time
	 */
	tw_stime trans_msg_ts;
	tw_stime GVT;
	tw_stime GVT_prev;
	tw_stime LVT;

#ifdef ROSS_GVT_mpi_allreduce
	tw_stat s_nwhite_sent;
	tw_stat s_nwhite_recv;
#endif
	/* start_time -- When this PE first started execution.
	 * end_time -- When this PE finished its execution.
	 */
	tw_wtime start_time;
	tw_wtime end_time;

	/* s_nevent_abort -- Number of events aborted.
	 * s_nsend_remote -- Number of events sent to another processor.
	 */
	tw_statistics	statistics;
	tw_stat s_nevent_abort;
	tw_stat s_nsend_net_remote;
	tw_stat s_nsend_loc_remote;
	tw_stat s_nsend_network;
	tw_stat s_nrecv_network;
	tw_stat s_nsend_remote_rb;
	tw_stat s_ngvts;

	/*
	 * rng  -- pointer to the random number generator on this PE
	 */
	tw_rng  *rng;

#ifndef ROSS_NETWORK_none
        /*
         * hash_t  -- array of incoming events from remote pes
         *            Note: only necessary for distributed DSR
         * seq_num  -- array of remote send counters for hashing on
         *                 size == g_tw_npe
         */
        void           *hash_t;
#ifdef ROSS_NETWORK_mpi
        tw_eventid	 seq_num;
#else
        tw_eventid	*seq_num;
#endif
#endif
};

DEF(struct, tw_log)
{
		struct
		{
				unsigned int
					rollback_primary:1,
					rollback_secondary:1,
					rollback_abort:1,
					send_event:1,
					recv_event:1,
					eventq_delete:1,
					send_cancel:1,
					recv_cancel:1,
					pq_enq:1,
					pq_deq:1,
					pq_delete:1,
					freeq_enq:1,
					freeq_deq:1,
					processed_enq:1,
					processed_deq:1;
		}
		state;

        tw_event        event;
        tw_event       *e;

#ifdef ROSS_THREAD_pthread
	pthread_t       tid;
#endif
        tw_log         *next;
        unsigned long long log_sz;
};

#define TW_MHZ 1000000
#endif
