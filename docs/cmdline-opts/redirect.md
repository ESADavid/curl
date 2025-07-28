---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: redirect
Arg: <mode>
Help: Custom method redirect behavior
Category: http
Added: 8.16.0
Multi: single
See-also:
  - request
  - location
Example:
  - -X POST --redirect spec $URL
---

# `--redirect`

Instructs curl to follow redirects and how to do the custom request method set
with --request when following redirects. Set the *mode* to use.

Use this option *instead of* --location.

## keep

The method string you set with --request is used for all requests even after
redirects. It may cause unintended side-effects when curl does not change
request method according to the HTTP 30x response codes - and similar.

This mode is what --location enables.

## spec

The method string you set with --request is used in subsequent requests for
the status codes 307 or 308, but may be reset to GET for 301, 302 and 303.

## init

The method string you set with --request is used only in the first outgoing
request but not in any additional requests, independently of what status code
is returned.
