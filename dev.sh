#!/usr/bin/env bash
set -euo pipefail

IMAGE_NAME="seawaves-dev"
TAG="latest"

docker build \
    --network host \
    --tag "$IMAGE_NAME:$TAG" \
    --output - \
    . \
    | tar -x --strip-components=1 app/bin/

gum log --time rfc822 --structured --level info Done
./bin/seawaves
