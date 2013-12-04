/*
 * Copyright (C) 2000-2011 Free Software Foundation, Inc.
 *
 * Author: Nikos Mavrogiannopoulos
 *
 * This file is part of GnuTLS.
 *
 * The GnuTLS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

int _gnutls_send_server_kx_message (gnutls_session_t session, int again);
int _gnutls_send_client_kx_message (gnutls_session_t session, int again);
int _gnutls_recv_server_kx_message (gnutls_session_t session);
int _gnutls_recv_client_kx_message (gnutls_session_t session);
int _gnutls_send_client_certificate_verify (gnutls_session_t session,
                                            int again);
int _gnutls_send_server_certificate (gnutls_session_t session, int again);
int _gnutls_generate_master (gnutls_session_t session, int keep_premaster);
int _gnutls_recv_client_certificate (gnutls_session_t session);
int _gnutls_recv_server_certificate (gnutls_session_t session);
int _gnutls_send_client_certificate (gnutls_session_t session, int again);
int _gnutls_recv_server_certificate_request (gnutls_session_t session);
int _gnutls_send_server_certificate_request (gnutls_session_t session,
                                             int again);
int _gnutls_recv_client_certificate_verify_message (gnutls_session_t session);
