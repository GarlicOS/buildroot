ARG base=20230207.1123
FROM registry.gitlab.com/buildroot.org/buildroot/base:${base}
COPY . .
ARG config
RUN make garlicos_${config}_defconfig
RUN make
