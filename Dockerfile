FROM ruby:3.1.4-alpine3.18

COPY build-deps /
RUN apk update && cat build-deps | xargs apk add --virtual build-dependencies

RUN mkdir /fast_json-schema
WORKDIR /fast_json-schema

ENV BUNDLE_PATH=/bundle \
    BUNDLE_BIN=/bundle/bin \
    GEM_HOME=/bundle
ENV PATH="${BUNDLE_BIN}:${PATH}"

COPY . ./

RUN gem install bundler
RUN bundle install --jobs 20 --retry 5
