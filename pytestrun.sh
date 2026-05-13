#!/bin/bash
set -e
cd "$(dirname "$0")"
PYTHONPATH=. pytest "$1"